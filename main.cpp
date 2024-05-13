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

Color DEEP_SPACE = (Color){10, 20, 30, 255};

int main()
{
    const int maxObjCount = 10;
    int objCount = 0;
    const int maxProjCount = 1000;
    int projCount = 0;

    bool LMBLast = false, RMBLast = false;

    InitWindow(window_width, window_height, "mygame");

    int frames = 0;
    SetTargetFPS(fps);
    cameraAnchor anchor;

    Ship s;

    Object* objArray = new Object[maxObjCount];
    // std::cout << objArray << " " << objArray + 1 << std::endl;
    s.init(objArray + 1);
    objCount = 2;

    Projectile* projArray = new Projectile[maxProjCount];
    
    objArray[0].initDefault();
    objArray[0].setMass(100);
    objArray[0].initHitboxDefault();
    

    UI ui;
      
    while(!WindowShouldClose())
    {
        BeginDrawing();

        Vector2 maxdist = s.getPos();
        maxdist.x = std::max(fabs(maxdist.x), fabs(objArray[0].getPos().x));
        maxdist.y = std::max(fabs(maxdist.y), fabs(objArray[0].getPos().y));

        BeginMode2D(anchor.cam);

        anchor.scale(maxdist);
        s.turnToMouse(anchor.getMouseScreenspaceCoords(), 1);

        for(int i=0; i<objCount; i++)
        {
            for(int j=0; j<objCount; j++)
                if(i != j)
                    objArray[i].checkCollision(objArray + j);

            for(int j=0; j<projCount; j++)
                objArray[0].checkCollision(&(projArray[j].obj));
        }

        s.move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));

        for(int i=0; i<1; i++)
            objArray[i].update();
        for(int i=0; i<projCount; i++)
            projArray[i].update();
            
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            s.fire(projArray + projCount);
            projCount ++;
        }

        ClearBackground(DEEP_SPACE);

        s.draw();
        objArray[0].draw();

        for(int i=0; i<projCount; i++)
            projArray[i].draw();

        //DrawRectangle(-2, -2, 4, 4, RAYWHITE);

        EndMode2D();

        ui.getParams(s.getVitals(), Vector3One());
        ui.draw();
        
        EndDrawing();

        frames++;
    }

    delete[](objArray);
    delete[](projArray);
    CloseWindow();

    return 0;
}