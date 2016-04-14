#pragma once

#include <vector>
#include <functional>
#include <memory>

using namespace std;

#include "Texture.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

// #include "World.hpp"

namespace game {
    class World;
    
    class Object {
    public:
        Object(gl::Mesh* geometry, gl::Program* program);
        ~Object();
        
        void Render ( void ) const;
        
        void Rotate(float angle, vec3 axis);
        void Translate(vec3 distance);
        
        class Component {
        public:
            bool enabled;
            Object* owner;
            virtual ~Component() {}
        
            mat4 const& Transform( void ) const { return owner->transform; }
        protected:
			Component(): enabled(true) {}
        };
            
        template<typename F>
        class Operation: public virtual Component, public std::function<F> {
        public:
            template <typename T>
            Operation(T&& _action): function<F>(std::forward<T>(_action)) {}
        protected:
            Operation() {}
        };
        
        Object& operator +=(Component* c);
        Object& operator -=(Component* c);
        
        class Renderer: public Operation<void( void )> {
        public:
            virtual void Render( void ) {if (*this) operator()(); }
            
            using Operation::Operation;
        };
        
        class Updater: public Operation<void(float)> {
        public:
            virtual void Update(float seconds) {if (*this) operator()(seconds); }
            
            using Operation::Operation;
        };
        
        class Collider: public Operation<void(Collider&)> {
        public:
            void CheckCollision(Object& other);
            virtual bool CollidesWith(Collider const& other) const = 0;
            virtual vec3 Exclude(Collider const& other) const { return vec3(); }
            
            using Operation::Operation;
        };
        
        class Interaction: public Operation<void(Collider&, Collider&)> {
        public:
            virtual void Resolve(Collider& source, Collider& other) { if (*this) operator()(source, other); }
            
            using Operation::Operation;
        };
        
        void CheckCollisions(Object& other);
        
        void Interact(Collider& element, Collider& other);
        
        void Update(float seconds);
        
        void Destroy( void );
        
        friend class World;
        
        vec3 Center( void ) const;
        
        template<typename T>
        vector<T*> Components() {
            vector<T*> found;
            for (Component* c: components) {
                T* test = dynamic_cast<T*>(c);
                if (test) found.push_back(test);
            }
            return found;
        }
        
        World& Host() { return *host; }
        
        vec4 color;
        gl::Texture* surface;
        
    protected:
        mat4 transform;
        gl::Mesh* geometry;
        gl::Program* program;
        
        World* host;
        
        vector<Component*> components;
    };

}