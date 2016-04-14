//
//  GL1.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/9/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include <iostream>
#include <list>

#include "SDL.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "World.hpp"
#include "Object.hpp"

#include "Image.hpp"

#include "Build.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using glm::vec3; using glm::mat4;

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keycode.h>

using namespace std;

class Display {
public:
    Display(unsigned int width, unsigned int height);
    void Swap( void ) const;
protected:
    SDL_Window* display;
};

Display::Display(unsigned int width, unsigned int height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    display = SDL_CreateWindow("", 20, 20, width, height, SDL_WINDOW_OPENGL);
    if (!display) throw sdl::Exception( "Failed to create window" );
    if (!SDL_GL_CreateContext(display)) throw sdl::Exception("Failed to create context");
    
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
#endif
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(vec4(1.0f)));
    
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    /* Setup our viewport. */
    glViewport( 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height) );
}

void Display::Swap() const {
    SDL_GL_SwapWindow(display);
}

class Project {
public:
    Project(unsigned int width, unsigned int height);
    Project& Build( void (*constructor) (game::World& w, gl::Program* program) );
    int Run( void );
protected:
    void Render( void );
    void ProcessKey(SDL_Keysym const& keysym);
    void Events();
    void ApplyTime(float seconds);
    
    Display display;
    bool running;
    game::World objects;
    gl::Program plain;
};

Project::Project(unsigned int width, unsigned int height):
    display(width, height),
    running(true),
    plain(gl::Shader(GL_VERTEX_SHADER, shader::vFlat), gl::Shader(GL_FRAGMENT_SHADER, shader::fFlat), gl::Vertex::Array::Bindings())
{
    plain.Uniform<mat4>("camera") = glm::lookAt(vec3(0.0f, 2.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    plain.Uniform<mat4>("projection") = glm::frustum(-2.0f,  2.0f, -1.5f, 1.5f, 02.0f, -5.0f);
    plain.Uniform<mat4>("transform") = glm::mat4();
}

Project& Project::Build(void (*constructor)(game::World &, gl::Program *)) {
    constructor(objects, &plain);
    return *this;
}

void Project::Render( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    objects.Render();
    
    display.Swap();
}

void Project::ProcessKey(SDL_Keysym const& keysym) {
    switch( keysym.sym ) {
        case SDLK_ESCAPE:
            running = false;
            break;
        default:
            break;
    }
}

void Project::Events() {
    /* Our SDL event placeholder. */
    SDL_Event event;
    
    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {
        
        switch( event.type ) {
            case SDL_KEYDOWN:
                /* Handle key presses. */
                ProcessKey( event.key.keysym );
                break;
            case SDL_QUIT:
                /* Handle quit requests (like Ctrl-c). */
                running = false;
                break;
        }
    }
}

void Project::ApplyTime(float seconds) {
    objects.Update(seconds);
}

int Project::Run( void ) {
    Uint32 lastCall = 0;
    while (running) {
        Events();
        Uint32 time = SDL_GetTicks();
        ApplyTime(static_cast<float>(time - lastCall) / 1000.0f);
        lastCall = time;
        Render();
    }
    return 0;
}

int main(int argc, char* argv[]) {
    static_cast<void>(argc), static_cast<void>(argv);
    
    try {
        sdl::Library SDL2(SDL_INIT_VIDEO);
        return Project(640, 480).Build(BuildWorld).Run();
    } catch (std::exception& e) {
        cerr << e.what() <<endl;
        return 1;
    }
}
