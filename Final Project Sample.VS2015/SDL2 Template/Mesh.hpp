//
//  World.hpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
using namespace std;

#include "OpenGL.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"

namespace gl {
    class Mesh {
    public:
        Mesh(string const& filename);
        
        void Render ( void ) const;
    protected:
        gl::Vertex::Array data;
        vector<GLushort> faces;
    };
}
