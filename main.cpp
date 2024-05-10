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
    InitWindow(window_width, window_height, "mygame");

    int frames = 0;
    SetTargetFPS(fps);
    cameraAnchor anchor;

    Ship s;
    Object o;
    o.setMass(100);
    o.initHitboxDefault();

    UI ui;
      
    while(!WindowShouldClose())
    {
        BeginDrawing();

        Vector2 maxdist = s.getPos();
        maxdist.x = std::max(fabs(maxdist.x), fabs(o.getPos().x));
        maxdist.y = std::max(fabs(maxdist.y), fabs(o.getPos().y));

        BeginMode2D(anchor.cam);

        anchor.scale(maxdist);
        s.turnToMouse(anchor.getMouseScreenspaceCoords(), 1);

        for(int subticks=0; subticks<physicsTicks; subticks++)
        {
            s.obj.checkCollision(&o);
            // o.checkCollision(&(s.obj));
            s.move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));
            o.update();
        }

        ClearBackground(DEEP_SPACE);

        s.draw();
        o.draw();

       

        DrawRectangle(-2, -2, 4, 4, RAYWHITE);

        EndMode2D();

        ui.getParams(s.getVitals(), Vector3One());
        ui.draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}