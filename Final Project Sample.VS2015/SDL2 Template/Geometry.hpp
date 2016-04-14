#pragma once

#include "Object.hpp"

namespace geometry {
    struct Sphere; struct Box;
    
    class Shape: public vec3 {
    public:
        virtual Shape& Transform(mat4 transform);
        
        class Collider: public game::Object::Collider {
        public:
            virtual bool IntersectShape(Sphere s) const = 0;
            virtual bool IntersectShape(Box b) const = 0;
            
            virtual vec3 FindExclusion(Sphere s) const = 0;
            virtual vec3 FindExclusion(Box b) const = 0;
        };
    protected:
        using vec3::vec3;
    };
        
    struct Sphere: public Shape {
        float radius;
        
        template<class ... Args>
        Sphere(float r, Args... a): Shape(a...), radius(r) {}
        
        Shape& Transform(mat4 transform);
    
        static Sphere Transform(Sphere s, mat4 transform);
    };
    
    struct Box: public Shape {
        vec3 extents;
        
        template<class ... Args>
        Box(vec3 xt, Args... a): Shape(a...), extents(xt) { PosQuad(extents); }
        
        template<class ... Args>
        Box(float w, float h, float d, Args... a): Shape(a...), extents(w/2, h/2, d/2) { PosQuad(extents); }
        
        Shape& Transform(mat4 transform);
        
        static Box Transform(Box b, mat4 transform);
        
    private:
        static void PosQuad(vec3& v) {
            if (v.x < 0) v.x = -v.x;
            if (v.y < 0) v.y = -v.y;
            if (v.z < 0) v.z = -v.z;
        }
    };
    
    template<typename S>
    class Collider: public Shape::Collider {
    public:
        bool CollidesWith(game::Object::Collider const& other) const {
            Shape::Collider const* otherShape = dynamic_cast<Shape::Collider const*>(&other);
            return otherShape != nullptr && otherShape->IntersectShape(S::Transform(definition, Transform()));
        }
        vec3 Exclude(game::Object::Collider const& other) const {
            Shape::Collider const* otherShape = dynamic_cast<Shape::Collider const*>(&other);
            return otherShape? otherShape->FindExclusion(S::Transform(definition, Transform())): vec3();
        }
    protected:
        template<class ... args>
        Collider(args... A): definition(A...) {}
        
        S definition;
    };
    
    class SphereCollider: public geometry::Collider<Sphere> {
    public:
        template<class ... args>
        SphereCollider(args... A): Collider<Sphere>(A...) {}
        
        bool IntersectShape(Sphere other) const;
        bool IntersectShape(Box other) const;
        
        vec3 FindExclusion(Sphere other) const;
        vec3 FindExclusion(Box other) const;
    };
    
    class BoxCollider: public geometry::Collider<Box> {
    public:
        template<class ... args>
        BoxCollider(args... A): Collider<Box>(A...) {}
        
        bool IntersectShape(Sphere other) const;
        bool IntersectShape(Box other) const;
        
        vec3 FindExclusion(Sphere other) const;
        vec3 FindExclusion(Box other) const;
    };
}