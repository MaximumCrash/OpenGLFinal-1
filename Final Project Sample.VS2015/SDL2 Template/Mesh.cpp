//
//  World.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "Mesh.hpp"

class OBJmesh {
public:
    OBJmesh(string const& filename);
    vector<gl::Vertex> const& vData() { return vertices; }
    vector<GLushort> const& fData() { return faces; }
    vector<GLushort> const& faceSegments() { return ranges; }
private:
    vector<gl::Vertex> vertices;
    vector<GLushort> faces;
    vector<GLushort> ranges;
    
    struct Index {
        GLshort p, t, n;
        Index(string const& encoding, GLshort pCount, GLshort tCount, GLshort nCount);
        
        bool operator==(Index const& other) const {
            return ((p == other.p) || (p < 0 && other.p < 0))
                && ((t == other.t) || (t < 0 && other.t < 0))
                && ((n == other.n) || (n < 0 && other.n < 0));
        }
    private:
        GLshort Enter(istream& source, GLshort limit) {
            GLshort result;
            if (source.peek() != '/' && source >> result) {
                result = result < 0? limit - result: --result;
            } else {
                result = -1;
            }
            source.ignore();
            return result;
        }
    };
    
    class hash: public std::hash<string> {
    public:
        size_t operator() (Index const i) const {
            return std::hash<string>::operator () (string(reinterpret_cast<char const*>(&i), sizeof(Index)));
        }
    };
};

OBJmesh::Index::Index(string const& encoding, GLshort pCount, GLshort tCount, GLshort nCount) {
    istringstream description(encoding);
    
    p = Enter(description, pCount);
    t = Enter(description, tCount);
    n = Enter(description, nCount);
}

OBJmesh::OBJmesh(string const& filename) {
    string line;
    vector<gl::Point3> vList;
    vector<gl::Vector3> vnList;
    vector<gl::Vector2> vtList;
    unordered_map<Index, GLshort, OBJmesh::hash> verts;
    for (ifstream source(filename); getline(source, line); ) {
        istringstream input (line);
        string command;
        input >> command;
        if (command == "v") {
            gl::Point3 v;
            input >> v.x >> v.y >> v.z;
            vList.push_back(v);
        } else if (command == "vt") {
            gl::Vector2 t;
            input >> t.x >> t.y;
            vtList.push_back(t);
        } else if (command == "vn") {
            gl::Vector3 n;
            input >> n.x >> n.y >> n.z;
            vnList.push_back(n);
        } else if (command == "f") {
            string corner;
            int corners = 0;
            while (input >> corner) {
                Index id(corner, vList.size(), vtList.size(), vnList.size());
                if (!verts.count(id)) {
                    gl::Vertex v;
                    if (id.p >= 0) v.position = vList[id.p];
                    if (id.t >= 0) v.uv = vtList[id.t];
                    if (id.n >= 0) v.normal = vnList[id.n];
                    verts[id] = vertices.size();
                    vertices.push_back(v);
                }
                corners++;
                faces.push_back(verts[id]);
            }
            ranges.push_back(corners);
        }
    }
}

vector<GLushort> loadShape(string const& filename, gl::Vertex::Array& vertexArray) {
    OBJmesh source(filename);

    gl::Buffer vbo, ebo;
    vbo.Load(GL_ARRAY_BUFFER, GL_STATIC_DRAW, source.vData());
    vertexArray.Bind(vbo);
    
    vertexArray.Activate();
    ebo.Load(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, source.fData());
    vertexArray.Deactivate();

	vbo.Release(); ebo.Release();
    
    return source.faceSegments();
}

namespace gl {
    Mesh::Mesh(string const& filename) {
        faces = loadShape(filename, data);
    }
    
    void Mesh::Render( void ) const {
        data.Activate();
        GLushort position = 0;
        for (GLushort range: faces) {
            glDrawElements(GL_TRIANGLE_STRIP, range, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(position * sizeof(GLshort)));
            position += range;
        }
        data.Deactivate();
    }
}
