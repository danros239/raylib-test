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

void combProjArray(Projectile* arr, size_t size)
{
    size_t start = 0, end = size;

}

const Color DEEP_SPACE = (Color){10, 20, 30, 255};

inline bool rectContains(Rectangle rect, Vector2 vec) // returns true if a point(vec) is inside the rectangle(rect)
{
    return((rect.x < vec.x && rect.x+rect.width > vec.x) && (rect.y < vec.y && rect.y+rect.height > vec.y));
}

void applyComponentToShip(Ship* ship, Component* comp, Vector2 targetPos)
{
    for(int i=0; i<ship->layout.lines; i++)
    {
        for(int j=0; j<ship->layout.columns; j++)
        {
            Rectangle rect = (Rectangle){ship->layout.offset.x + j*30.0f, 
                                         ship->layout.offset.y + i*30.0f, 25, 25};
            if(rectContains(rect, targetPos))
                ship->layout.addComponent(i, j, *comp);
        }    
    }
}

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
    sceneType type;
    ~Scene()
    {
        std::cout << "~Scene()" << std::endl;
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
    Ship* shipArray = new Ship[100];
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

        // Hitbox defbox;
        // defbox.initFromFile("data/default.box");

        Component gun(1);

        shipArray[0].init(objArray + objCount);
        objCount++;
        shipArray[0].id = 0;
        shipArray[0].layout.addComponent(0, 2, gun);

        shipArray[1].init(objArray + objCount);
        objCount++;

        shipArray[1].id = 1;
        objArray[1].setPos((Vector2){100, 0});
    }
    void processInput()
    {
        shipArray[0].turnToMouse(cam.getMouseScreenspaceCoords(), !IsKeyDown(KEY_LEFT_CONTROL));
        shipArray[0].move(IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsKeyDown(KEY_S), IsKeyDown(KEY_A), IsKeyDown(KEY_D));
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) // more keybinds soon
        {
            for(int i=0; i<shipArray[0].layout.componentArray.size(); i++)
            {
                if(shipArray[0].layout.componentArray[i].active)
                // for every active component of controlled ship
                {
                    shipArray[0].fire((projArray+projCount), shipArray[0].layout.componentArray[i].ID);
                    projCount++;
                }
            }

        }
    }
    void updatePhysics()
    {
        for(int i=0; i<objCount; i++)
        {
            // check for object-object collisions
            for(int j=0; j<objCount; j++)
            {
                if(j != i)
                    objArray[i].checkCollision(&objArray[j]);
            }
            // check for object-projectile collisions
            for(int j=0; j<projCount; j++)
            {
                if(i != projArray[j].spawnedBy) // friendly fire protection
                    if(objArray[i].checkCollision(&projArray[j].obj))
                    {
                        projArray[j].active = false;
                        shipArray[i].hurt(projArray[j].damage);
                    }
            }
            objArray[i].update();
        }

        for(int i=0; i<projCount; i++)
            projArray[i].update();
        std::cout << projCount << std::endl;

        ui.getParams(shipArray[0].getVitals(), shipArray[1].getVitals());
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

        shipArray[0].draw();

        EndMode2D();
        ui.draw();
        EndDrawing();
    }

    ~SceneCombat()
    {
        if(objCount != 0)
            delete[](objArray);
        if(projCount != 0)
            delete[](projArray);
        delete[](shipArray);
    }
};

class SceneEditor : public Scene
{
public:
    Ship currentShip;
    Object o;
    cameraAnchor cam;
    Component comp;
    SceneEditor()
    {

    }
    void init()
    {
        currentShip.init(&o);
        currentShip.obj->setAngle(-PI*0.5);
        currentShip.obj->setPos((Vector2){0, 10});
        currentShip.obj->update();
        cam.scale(Vector2Zero(), (Vector2){0, 0});
    }
    void processInput()
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            applyComponentToShip(&currentShip, &comp, cam.getMouseScreenspaceCoords());
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            currentShip.layout.removeComponent(0, 0);
    }
    void updatePhysics()
    {
        
    }
    void draw()
    {
        BeginDrawing();
        BeginMode2D(cam.cam);

        ClearBackground(DEEP_SPACE);
        currentShip.drawLayout();

        Vector2 mousePos = cam.getMouseScreenspaceCoords();

        DrawText((std::to_string(mousePos.x) + std::to_string(mousePos.y)).c_str(), mousePos.x, mousePos.y, 10, RAYWHITE);
        
        EndMode2D();
        EndDrawing();
    }
    ~SceneEditor()
    {

    }
};