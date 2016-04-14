//
//  OpenGL.h
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/29/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <cstddef>

#include "glm/glm.hpp"
using glm::vec2; using glm::vec3; using glm::vec4; using glm::mat4;

namespace gl {
    template <class T>
    class Name {
    public:
        Name(Name && source) { name = source.name; source.name = 0; }
        operator bool() { return name != 0; }
        Name& operator= (Name && source) { name = source.name; source.name = 0; return *this; }
    protected:
        Name();
        Name(GLuint existing): name(existing) {}
        ~Name();
        
        GLuint name;
    };
    
    typedef vec3 Point3;
    typedef vec3 Vector3;
    typedef vec2 Vector2;
}

namespace shader {
    extern std::string vFlat;
    extern std::string fFlat;
    extern std::string vPhong;
    extern std::string fPhong;
}
