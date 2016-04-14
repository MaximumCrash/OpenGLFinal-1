//
//  World.cpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 2/26/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "World.hpp"

#include "Object.hpp"

namespace game {
    World::World():
        time(0.0f)
    {}

    World& World::operator +=(Object* what) {
        if (what->host && what->host != this) *what->host -= what;
        push_back(what);
        what->host = this;
        return *this;
    }

    World& World::operator -=(Object *what) {
        remove(what);
        what->host = nullptr;
        return *this;
    }
    
    void World::Destroy(Object* what) {
        destroyed.push_back(what);
    }
    
    void World::CheckCollision(game::Object* what) {
        moved.push_back(what);
    }
    
    bool World::LogCollision(Object::Collider &a, Object::Collider &b) {
        int additions = 0;
        if (collisions.count(&a) == 0) collisions[&a] = unordered_set<Object::Collider*>();
        if (collisions.count(&b) == 0) collisions[&b] = unordered_set<Object::Collider*>();
        if (collisions[&a].count(&b) == 0) { ++additions; collisions[&a].insert(&b); }
        if (collisions[&b].count(&a) == 0) { ++additions; collisions[&b].insert(&a); }
        return additions > 0;
    }

    void World::Update(float seconds) {
        time += seconds;
        for (Object* obj: *this) obj->Update(seconds);
        vector<Object*> check = std::move(moved);
        moved.clear();
        for (Object* body: check) {
            for (Object* reference: *this) body->CheckCollisions(*reference);
        }
        unordered_map<Object::Collider*, unordered_set<Object::Collider*>> hits;
        std::swap(hits, collisions);
        for (auto list: hits) {
            for (auto partner: list.second) {
                list.first->owner->Interact(*list.first, *partner);
            }
        }
        for (Object* dead: destroyed) delete dead;
        destroyed.clear();
    }

    void World::Render( void ) const {
        for (Object* object: *this) {
            object->Render();
        }
    }
}