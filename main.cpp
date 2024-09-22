#include <iostream>

#ifndef RAYLIB_H
#include <raylib.h> 
#define RAYLIB_H
#endif

#ifndef ENGINE
#include "engine.hpp"
#define ENGINE
#endif  

#ifndef OBJECT
#include "object.hpp"
#define OBJECT
#endif

#include "scene.hpp"

int main()
{
    // SetConfigFlags(FLAG_MSAA_4X_HINT); 
    InitWindow(window_width, window_height, "mygame");
    

    int frames = 0;
    SetTargetFPS(fps);

    Scene* sc = new SceneCombat();
    // Scene* sc = new SceneEditor();
    sc->init();
      
    while(!WindowShouldClose())
    {
        sc->processInput();
        sc->updatePhysics();
        sc->draw();

        frames++;
    }
    delete(sc);
    CloseWindow();

    return 0;
}