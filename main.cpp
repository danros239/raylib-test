#include <iostream>

#ifndef RAYLIB_H
#include <raylib.h>
#define RAYLIB_H
#endif

#ifndef ENGINE
#include "engine.hpp"
#define ENGINE
#endif

#include "object.hpp"

const Color DEEP_SPACE = (Color){10, 20, 30, 255};

enum sceneType
    {
        MAIN_MENU,
        COMBAT,
        EDITOR
    };

class Scene
{
    enum Type
    {
        MAIN_MENU,
        COMBAT,
        EDITOR
    };
};

int main()
{
    const int maxObjCount = 10;
    int objCount = 0;
    const int maxProjCount = 1000;
    int projCount = 0;

    bool LMBLast = false, RMBLast = false;

    sceneType currentScene = sceneType::COMBAT;

    InitWindow(window_width, window_height, "mygame");

    int frames = 0;
    SetTargetFPS(fps);
    cameraAnchor anchor;

    Ship playerShip;

    Object* objArray = new Object[maxObjCount];

    playerShip.init(objArray + 1);
    objCount = 2;

    Projectile* projArray = new Projectile[maxProjCount];
    
    objArray[0].initDefault();
    objArray[0].setMass(100);
    objArray[0].initHitboxDefault();

    UI ui;
      
    while(!WindowShouldClose())
    {
        BeginDrawing();
        switch(currentScene)
        {
        case sceneType::COMBAT:
            Vector2 maxdist = playerShip.getPos();
            maxdist.x = std::max(fabs(maxdist.x), fabs(objArray[0].getPos().x));
            maxdist.y = std::max(fabs(maxdist.y), fabs(objArray[0].getPos().y));

            BeginMode2D(anchor.cam);

            anchor.scale(maxdist);
            playerShip.turnToMouse(anchor.getMouseScreenspaceCoords(), 1);

            for(int i=0; i<objCount; i++)
            {
                for(int j=0; j<objCount; j++)
                    if(i != j)
                        objArray[i].checkCollision(objArray + j);

                for(int j=0; j<projCount; j++)
                    objArray[0].checkCollision(&(projArray[j].obj));
            }

            playerShip.move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));

            for(int i=0; i<1; i++)
                objArray[i].update();
            for(int i=0; i<projCount; i++)
                projArray[i].update();
                
            if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                playerShip.fire(projArray + projCount);
                projCount ++;
            }

            ClearBackground(DEEP_SPACE);

            playerShip.draw();
            objArray[0].draw();

            for(int i=0; i<projCount; i++)
                projArray[i].draw();

            //DrawRectangle(-2, -2, 4, 4, RAYWHITE);

            EndMode2D();

            ui.getParams(playerShip.getVitals(), Vector3One());
            ui.draw();
            
            break;
        }
        EndDrawing();
        frames++;
    }

    delete[](objArray);
    delete[](projArray);
    CloseWindow();

    return 0;
}