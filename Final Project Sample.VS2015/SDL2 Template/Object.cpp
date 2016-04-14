#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Object.hpp"
#include "World.hpp"

namespace game {
    Object::Object(gl::Mesh* m, gl::Program* prog):
        geometry(m),
        program(prog),
        color(1.0f),
        surface(nullptr),
        host(nullptr)
    {}
    
    Object::~Object() {
        if (host) *host -= this;
        for (Component* component: components) delete component;
    }
    
    void Object::Collider::CheckCollision(Object& other) {
        for (Collider* otherCollider: other.Components<Collider>()) {
            if (otherCollider->enabled && CollidesWith(*otherCollider)) {
                owner->Host().LogCollision(*this, *otherCollider);
            }
        }
    }
    
    void Object::Render( void ) const {
        program->Activate();
        program->Uniform<vec4>("color") = color;
        program->Uniform<GLint>("tx") = surface? surface->Activate(): gl::Texture::Deactivate();
        program->Uniform<mat4>("transform") = transform;
        geometry->Render();
    }
    
    void Object::Rotate(float angle, vec3 axis) {
        transform = glm::rotate(transform, angle, axis);
    }
    
    void Object::Translate(vec3 distance) {
        transform = glm::translate(transform, distance);
        if (host) host->CheckCollision(this);
    }
    
    vec3 Object::Center( void ) const {
        static vec4 const origin(0.0f, 0.0f, 0.0f, 1.0f);
        
        return vec3(transform * origin);
    }
    
    Object& Object::operator +=(Component* c) {
        components.push_back(c);
        c->owner = this;
        return *this;
    }
    
    Object& Object::operator -=(Component* c) {
        components.erase(find(components.begin(), components.end(), c));
        c->owner = nullptr;
        return *this;
    }
    
    void Object::CheckCollisions(Object& other) {
        if (this == &other) return;
        for (Collider* collider: Components<Collider>()) {
            if (collider->enabled) collider->CheckCollision(other);
        }
    }
    
    void Object::Interact(Object::Collider& element, Object::Collider& other) {
        for (Interaction* effect: Components<Interaction>()) {
            if (effect->enabled) effect->Resolve(element, other);
        }
    }
    
    void Object::Update(float seconds) {
        for (Updater* action: Components<Updater>()) {
            if (action->enabled) action->Update(seconds);
        }
    }
    
    void Object::Destroy( void ) {
        host->Destroy(this);
    }
}