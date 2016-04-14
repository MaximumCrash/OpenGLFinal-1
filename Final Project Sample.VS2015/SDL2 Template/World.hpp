#pragma once

#include <list>
#include <vector>
#include <unordered_set>

#include "OpenGL.hpp"

#include "Object.hpp"

namespace game {
    class Object;
    
    class World: protected std::list<Object*> {
    public:
        World();
        
        World& operator +=(Object* what);
        World& operator -=(Object* what);
        
        void Destroy(Object* what);
        
        void Update(float seconds);
        
        void CheckCollision(Object* what);
        bool LogCollision(Object::Collider& a, Object::Collider& b);
        
        void Render( void ) const;
        
        vec3 gravity;
    protected:
        float time;
        std::vector<Object*> destroyed;
        std::vector<Object*> moved;
        std::unordered_map<Object::Collider*, std::unordered_set<Object::Collider*> > collisions;
    };
}