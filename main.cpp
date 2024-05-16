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

enum sceneType
    {
        MAIN_MENU,
        COMBAT,
        EDITOR
    };


int main()
{
    sceneType currentScene = sceneType::COMBAT;

    InitWindow(window_width, window_height, "mygame");

    int frames = 0;
    SetTargetFPS(fps);

    Scene* sc = new SceneCombat();
    sc->init();
      
    while(!WindowShouldClose())
    {
        // BeginDrawing();
        // switch(currentScene)
        // {
        // case sceneType::COMBAT:
        //     Vector2 maxdist = playerShip.getPos();
        //     maxdist.x = std::max(fabs(maxdist.x), fabs(objArray[0].getPos().x));
        //     maxdist.y = std::max(fabs(maxdist.y), fabs(objArray[0].getPos().y));

        //     BeginMode2D(anchor.cam);

        //     anchor.scale(maxdist);
        //     playerShip.turnToMouse(anchor.getMouseScreenspaceCoords(), 1);

        //     for(int i=0; i<objCount; i++)
        //     {
        //         for(int j=0; j<objCount; j++)
        //             if(i != j)
        //                 objArray[i].checkCollision(objArray + j);

        //         for(int j=0; j<projCount; j++)
        //             objArray[0].checkCollision(&(projArray[j].obj));
        //     }

        //     playerShip.move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));

        //     for(int i=0; i<1; i++)
        //         objArray[i].update();
        //     for(int i=0; i<projCount; i++)
        //         projArray[i].update();
                
        //     if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        //     {
        //         playerShip.fire(projArray + projCount);
        //         projCount ++;
        //     }

        //     ClearBackground(DEEP_SPACE);

        //     playerShip.draw();
        //     objArray[0].draw();

        //     for(int i=0; i<projCount; i++)
        //         projArray[i].draw();

        //     //DrawRectangle(-2, -2, 4, 4, RAYWHITE);

        //     EndMode2D();

        //     ui.getParams(playerShip.getVitals(), Vector3One());
        //     ui.draw();
            
        //     break;
        // }
        // EndDrawing();


        sc->processInput();
        sc->updatePhysics();

        sc->draw();

        frames++;
    }
    delete(sc);
    CloseWindow();

    return 0;
}