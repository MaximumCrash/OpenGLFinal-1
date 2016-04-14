//
//  Vertex.hpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/31/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <string>
#include <unordered_map>
using namespace std;

#include "OpenGL.hpp"
#include "Buffer.hpp"

namespace gl {
    struct Vertex {
        Point3 position;
        Vector3 normal;
        Vector2 uv;
        
        class Array: public Name<Vertex::Array> {
        public:
            Array();
            
            Array& Bind(Buffer& storage);
            
            void Activate() const;
            
            static void Deactivate();
            
            const static GLint position = 0, normal = 1, uv = 2;
            
            static unordered_map<string, GLint> const& Bindings( void );
            
        private:
            static unordered_map<string, GLint> bindings;
        };
    };
}
