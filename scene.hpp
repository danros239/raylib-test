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

const Color DEEP_SPACE = (Color){10, 20, 30, 255};

class Scene
{
private:
    enum sceneType
    {
        MAIN_MENU,
        EDITOR,
        COMBAT,
        WORLD_MAP
    };
public:
    ~Scene()
    {
    
    };
    virtual void init() {};
    virtual void processInput() {};
    virtual void updatePhysics() {};
    virtual void draw() {};
    virtual void stop () {};

};

class SceneCombat : public Scene
{
public:
    Object* objArray;
    Projectile* projArray;
    Ship playerShip;
    size_t objCount, projCount;
    UI ui;

    cameraAnchor cam;
    SceneCombat()
    {
        objArray = nullptr;
        projArray = nullptr;
        objCount = 0;
        projCount = 0;
    };
    void init()
    {
        objArray = new Object[20];
        projArray = new Projectile[100];
        playerShip.init(objArray + objCount);
        objCount++;
        objArray[1].initDefault();
        objArray[1].setPos((Vector2){100, 0});
        objCount++;
    }
    void processInput()
    {
        playerShip.turnToMouse(cam.getMouseScreenspaceCoords(), !IsKeyDown(KEY_LEFT_CONTROL));
        playerShip.move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            playerShip.fire(&projArray[projCount]);
            projCount++;
        }
        ui.getParams(playerShip.getVitals(), Vector3One());
    }
    void updatePhysics()
    {
        for(int i=0; i<objCount; i++)
        {
            for(int j=0; j<objCount; j++)
            {
                if(j != i)
                    objArray[i].checkCollision(&objArray[j]);
            }
            for(int j=0; j<projCount; j++)
            {
                objArray[i].checkCollision(&projArray[j].obj);
            }
            objArray[i].update();
        }

        for(int i=0; i<projCount; i++)
            projArray[i].update();
    }
    void draw()
    {
        Vector2 maxCoords = objArray[0].getPos(), minCoords = maxCoords;
        for(int i=0; i<objCount; i++)
        {
            maxCoords.x = std::max(objArray[i].getPos().x, maxCoords.x);
            maxCoords.y = std::max(objArray[i].getPos().y, maxCoords.y);

            minCoords.x = std::min(objArray[i].getPos().x, minCoords.x);
            minCoords.y = std::min(objArray[i].getPos().y, minCoords.y);
        }
        
        cam.scale(minCoords, maxCoords);
        BeginDrawing();
        BeginMode2D(cam.cam);

        ClearBackground(DEEP_SPACE);
        DrawPixelV(Vector2Zero(), RED);

        for(int i=0; i<objCount; i++)
            objArray[i].draw();
        for(int i=0; i<projCount; i++)
            projArray[i].draw();
        
        EndMode2D();
        ui.draw();
        EndDrawing();
    }
    void stop()
    {

    }
    ~SceneCombat()
    {
        if(objCount != 0)
            delete[](objArray);
        if(projCount != 0)
            delete[](projArray);
    }
};