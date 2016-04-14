#include "Geometry.hpp"

#include "glm/gtx/norm.hpp"

namespace geometry {
    Shape& Shape::Transform(mat4 transform) {
        vec3::operator=( vec3(transform * vec4(*this, 1.0f)) );
        return *this;
    }
    
    Shape& Sphere::Transform(mat4 transform) {
        Shape::Transform(transform);
        radius = glm::length(vec3(transform[3] * vec4(glm::normalize(vec3(1.0f)) * radius, 1.0f)));
        return *this;
    }
    
    Sphere Sphere::Transform(Sphere s, mat4 transform) {
        s.Transform(transform);
        return s;
    }
    
    Shape& Box::Transform(mat4 transform) {
        Shape::Transform(transform);
        return *this;
    }
    
    Box Box::Transform(Box b, mat4 transform) {
        b.Transform(transform);
        return b;
    }
    
    namespace {
        bool BoxHitsSphere(Sphere sp, Box b) {
            sp -= static_cast<vec3&>(b);
            float s, d = 0;
            for (int i = 0; i < 3; ++i) {
                if (sp[i] < -b.extents[i]) {
                    s = sp[i] + b.extents[i];
                    d += s * s;
                } else if (sp[i] > b.extents[i]) {
                    s = sp[i] - b.extents[i];
                    d += s * s;
                }
            }
            return d <= sp.radius * sp.radius;
        }
        
        vec3 BoxApartSphere(Sphere sp, Box b) {
            sp -= static_cast<vec3&>(b);
            vec3 displacement = sp;
            bool inside = true;
            for (int i = 0; i < 3; ++i) {
                if (sp[i] < -b.extents[i]) {
                    displacement[i] = -b.extents[i];
                    inside = false;
                } else if (sp[i] > b.extents[i]) {
                    displacement[i] = b.extents[i];
                    inside = false;
                }
            }
            if (inside) {
                int index = 0;
                for (int i = 1; i < 3; ++i) {
                    float gap = b.extents[i] - abs(sp[i]);
                    if (gap < b.extents[index]) {
                        index = i;
                    }
                }
                displacement[index] = b.extents[index];
            }
            displacement = sp - displacement;
            return glm::normalize(displacement) * (sp.radius - glm::length(displacement));
        }
    }
    
    bool SphereCollider::IntersectShape(Sphere other) const {
        other.Transform(glm::inverse(Transform()));
        float distance = definition.radius + other.radius;
        return distance * distance >= glm::length2(static_cast<vec3>(other));
    }
    
    bool SphereCollider::IntersectShape(Box other) const {
        return BoxHitsSphere(Sphere::Transform(definition, Transform()), other);
    }
    
    vec3 SphereCollider::FindExclusion(Sphere other) const {
        Sphere self = Sphere::Transform(definition, Transform());
        vec3 offset = self - other;
        float distance = glm::length(offset), size = self.radius + other.radius;
        if (distance >= size) return vec3(0.0f);
        return glm::normalize(offset) * (size - distance);
    }
    
    vec3 SphereCollider::FindExclusion(Box other) const {
        return BoxApartSphere(Sphere::Transform(definition, Transform()), other);
    }
    
    bool BoxCollider::IntersectShape(Sphere other) const {
        return BoxHitsSphere(other, Box::Transform(definition, Transform()));
    }
    
    bool BoxCollider::IntersectShape(Box other) const {
        other.Transform(glm::inverse(Transform()));
        for (int i = 0; i < 3; ++i) {
            if (definition.extents[i] + other.extents[i] < abs(other[i] - definition[i])) {
                return false;
            }
        }
        return true;
    }
    
    vec3 BoxCollider::FindExclusion(Sphere other) const {
        return -BoxApartSphere(other, Box::Transform(definition, Transform()));
    }
    
    vec3 BoxCollider::FindExclusion(Box other) const {
        vec3 displacement;
        other.Transform(glm::inverse(Transform()));
        for (int i = 0; i < 3; ++i) {
            float offset = other[i] - definition[i];
            offset = copysign((other.extents[i] + definition.extents[i]) - abs(offset), offset);
            for (int j = 0; j < i; ++j) {
                if (displacement[j] != 0.0f) {
                    if (abs(displacement[j]) > abs(offset)) {
                        displacement[j] = 0.0f;
                    } else {
                        offset = 0.0f;
                    }
                }
            }
            displacement[i] = offset;
        }
        return displacement;
    }
}