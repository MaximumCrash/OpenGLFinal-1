#include "Build.hpp"

#include "Object.hpp"
#include "Image.hpp"
#include "Geometry.hpp"

#include <SDL2/SDL.h>

namespace physics {
    class Velocity: public game::Object::Updater {
    public:
        vec3 speed;
        Velocity(vec3 delta): Updater(), speed(delta) {}
        void Update(float seconds) {
            owner->Translate(speed * seconds);
        }
    };
    
    class Gravity: public game::Object::Updater {
    public:
        Gravity() {}
        
        void Update(float seconds) {
            for (Velocity* velocity: owner->Components<Velocity>()) {
                velocity->speed += owner->Host().gravity * seconds;
            }
        }
    };

    class Static: public game::Object::Interaction {
    public:
        float bounce;
        Static(float b = 0.96f): bounce(b) {}
        void Resolve(game::Object::Collider& element, game::Object::Collider& other) {
            vec3 ex = element.Exclude(other);
            other.owner->Translate(ex);
            vec3 normal = glm::normalize(ex);
            
            for (Velocity* velocity: other.owner->Components<Velocity>()) {
                velocity->speed = glm::reflect(velocity->speed, normal) * bounce;
            }
        };
    };
}

namespace utility {
	class Motor : public physics::Velocity {
	public:
		using Velocity::Velocity; 

		void Update(float seconds) {
			auto keys = SDL_GetKeyboardState(nullptr);
			if (keys[SDL_SCANCODE_LEFT]) {
				Velocity::Update(-seconds);
			}
			else if (keys[SDL_SCANCODE_RIGHT]) {
				Velocity::Update(seconds);
			}
			else if (keys[SDL_SCANCODE_UP]){

			}
			else if (keys[SDL_SCANCODE_DOWN]) {

			}
		}
	};

	class Timer : public game::Object::Updater {
	public:
		float duration;

		template<typename T>
		Timer(float t, T&& _action) : action(_action), duration(t) {}

		void Update(float seconds) {
			duration -= seconds;
			if (duration -= 0.0f) {
				action(*this);
				Expire();
			}
		}

	protected:
		virtual void Expire(void) {
			*owner -= this;
			delete this;
		}
	private:
		std::function<void(Timer&)> action;
	};

	class Repeater : public Timer {
	public:
		float const interval;

		template<typename T>
		Repeater(float t, T&& _action) : Timer(t, _action), interval(t) {}

	protected:
		void Expire(void) {
			if (duration <= 0) duration += interval;
		}
	};
}

void BuildWorld(game::World& world, gl::Program* program) {
    world.gravity = vec3(0.0f, -2.0f, 0.0f);
    
    auto octo = new gl::Mesh("cube.obj");
    
    gl::Mesh* block = new gl::Mesh("block.obj");
    
    gl::Texture *sand = new gl::Texture(image::TGA("sandstone.tga"));
    gl::Texture *moss = new gl::Texture(image::TGA("lichen.tga"));
	for (int i = 0; i < 10; i++)
	{
		game::Object* shape = new game::Object(octo, program);
		shape->Translate(vec3(-10.0f+(i*2), 2.0f, -10.0f));
		shape->surface = moss;
		shape->color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		*shape += new physics::Velocity(vec3(0.0f, 0.0f, 0.0f));
		//*shape += new physics::Gravity();
		*shape += new geometry::SphereCollider(0.5f);
		world += shape;
	}
	
	game::Object* shape = new game::Object(octo, program);
    shape->Translate(vec3(0.0f, -2.0f, -10.0f));
    shape->surface = sand;
	*shape += new utility::Motor(vec3(0.0f, 10.0f, 0.0f));
    *shape += new geometry::SphereCollider(0.5f);
    *shape += new physics::Static(0.0f);
    world += shape;
	
}