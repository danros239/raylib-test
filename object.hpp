#ifndef ENGINE
#include "engine.hpp"
#define ENGINE
#endif

#include <vector>

const int window_width = 1280, window_height = 720;

float softScale(float val, float mag)
{
    return sqrt(val*val + mag*mag);
}

inline void DrawMyRect(Rectangle rect, Color col)
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, col);
}

const Vector2 shipVertices[] = {
                        { 20, 0},
                        {-20, 0},
                        { 4, 5},
                        {-10, 14},
                        {4, -5},
                        {-10, -14}
};
const int shipIndices[] = {
    0, 1, 2,
    1, 0, 4,
    1, 3, 2,
    1, 4, 5
};

class cameraAnchor
{
private:
    Vector2 position; // position of camera's center
    const float minScale = 0.3, maxScale = 3;

public:
    Camera2D cam;
    cameraAnchor()
    {
        position = (Vector2){0, 0};
        cam.target = Vector2Zero();
        cam.rotation = 0;
        cam.zoom = 1;
        cam.offset = Vector2Zero();
    }
    void scale(Vector2 farthestCoords)
    {

        float xscale = window_width / 2 / softScale(farthestCoords.x, 100);
        float yscale = window_height / 2 / softScale(farthestCoords.y, 100);

        float zoom = std::min(xscale, yscale);
        cam.zoom = Clamp(zoom, minScale, maxScale);

        cam.offset = (Vector2){window_width/2, window_height/2};
    }
    void scale (Vector2 minCoords, Vector2 maxCoords)
    {
        cam.target = Vector2Scale(Vector2Add(minCoords, maxCoords), 0.5f);

        float xscale = window_width / softScale(maxCoords.x - minCoords.x, 200);
        float yscale = window_height / softScale(maxCoords.y - minCoords.y, 200);

        float zoom = std::min(xscale, yscale);
        cam.zoom = Clamp(zoom, minScale, maxScale);
        cam.offset = (Vector2){window_width/2, window_height/2};
    }
    void reset()
    {
        cam.offset = Vector2Zero();
        cam.zoom = 1;
    }
    Vector2 getMouseScreenspaceCoords()
    {
        Vector2 mouseCoords = Vector2Subtract(GetMousePosition(), cam.offset);
        return Vector2Add(Vector2Scale(mouseCoords, 1/cam.zoom), cam.target);
    }
};

class UI
{
private:
    float width = window_width / 4;
    Vector2 offset = (Vector2){120, 20};
    Vector3 allyVitals, enemyVitals;

    Rectangle allyHPBar, allyEnergyBar;
    Rectangle enemyHPBar, enemyEnergyBar;

public:
    UI()
    {
        allyVitals = Vector3One();
        enemyVitals = Vector3One();

        allyHPBar = (Rectangle){offset.x, offset.y, width, 10};
        enemyHPBar = (Rectangle){window_width - width - offset.x, offset.y, width, 10};

        allyEnergyBar = (Rectangle){offset.x, offset.y + 15, width, 5};
        enemyEnergyBar = (Rectangle){window_width - width - offset.x, offset.y + 15, width, 5};
    }
    Rectangle rectToScreenspace(Rectangle rect, Camera2D cam)
    {
        Vector2 pos = Vector2Scale(Vector2Subtract((Vector2){rect.x, rect.y}, cam.offset), 1/cam.zoom);
        Vector2 size = Vector2Scale((Vector2){rect.width, rect.height}, 1/cam.zoom);
        return (Rectangle){pos.x, pos.y, size.x, size.y};
    }

    void getParams(Vector3 ally, Vector3 enemy)
    {
        allyVitals = ally;
        enemyVitals = enemy;
    }

    void draw()
    {
        allyEnergyBar.width = allyVitals.x*width;
        allyHPBar.width = allyVitals.y*width;

        enemyEnergyBar.width = enemyVitals.x*width;
        enemyHPBar.width = enemyVitals.y*width;
        //enemyEnergyBar.x = (1-enemyVitals.x)*width;

        DrawMyRect(allyEnergyBar, YELLOW);
        DrawMyRect(allyHPBar, GREEN);

        DrawMyRect(enemyEnergyBar, YELLOW);
        DrawMyRect(enemyHPBar, GREEN);
    }
};

class Component
{
public:
    unsigned int ID;
    size_t line, column;
    char type;
    bool active;
    int cooldown = 0;
    KeyboardKey keyBinding;
    Component()
    {
        ID = 0;
        line = 0;
        column = 0;
        active = false;
        keyBinding = KeyboardKey::KEY_ZERO;
        type = '0';
    }
    Component(unsigned int id)
    {
        ID = id;
        switch (id)
        {
        case 1:
            line = 0;
            column = 0;
            active = true;
            //keyBinding = MouseButton::MOUSE_BUTTON_LEFT;
            type = 'r';
            break;
        
        default:
            line = 0;
            column = 0;
            active = false;
            type = 'g';
            break;
        }

    }
    void activate()
    {
        switch(ID)
        {
        case 1:
            cooldown = 60;

            break;
        
        default:
            break;
        }
    }
};

class Layout
{
private:
    int state;
public:
    std::string* layoutArray;
    bool* occupied;
    std::vector<Component> componentArray;
    size_t lines, columns;
    Vector2 offset;
    enum LayoutColors
    {
        
    };
    Layout()
    {
        state = 0;
        lines = 0;
        columns = 0;
        layoutArray = nullptr;
        occupied = nullptr;
        offset = (Vector2){0, 0};
    }
    void initDefault()
    {
        lines = 7;
        columns = 5;
        offset = (Vector2){columns * -15.0f + 2.5f, lines*-15.0f + 30.f};
        layoutArray = new std::string[7]{"00r00",
                                         "00r00", 
                                         "0brb0", 
                                         "0ggg0", 
                                         "bgggb", 
                                         "0yyy0", 
                                         "00y00"};

        occupied = new bool[columns*lines];
        for(int i=0; i<columns*lines; i++)
        {
            occupied[i] = (layoutArray[i/columns][i%columns] == '0');
        }
    }
    void addComponent(size_t line, size_t column, Component comp)
    {
        if(!occupied[line*columns + column])
        {
            comp.line = line;
            comp.column = column;
            componentArray.push_back(comp);
            occupied[line*columns + column] = true;
            
        }
    }
    void removeComponent(size_t line, size_t column)
    {
        if(!componentArray.empty())
        {
            Component last = componentArray.back();
            componentArray.pop_back();
            occupied[last.line * columns + last.column] = false;
        }
    }
    ~Layout()
    {
        delete[](layoutArray);
        delete[](occupied);
    }
};

class Projectile // Anything fired from a ship's gun
{
public:
    Object obj;
    enum Behavior
    {
        KINETIC, BEAM
    };
    Behavior type;

    float lifetime, damage, velocity;
    bool active;
    size_t spawnedBy;

    Projectile()
    {
        type = KINETIC;
        spawnedBy = 0;
        active = false;
        damage = 0;
        velocity = 0;
    }
    void init()
    {
        obj.initDefault();
        obj.initHitboxPoint();
        active = true;
    }
    void init(Behavior typ)
    {
        switch(typ)
        {
        default:
            obj.initDefault();
            obj.initHitboxPoint();
            obj.setMass(1);

            damage = 10;
            type = typ;
            lifetime = 2;
            active = true;
            
            break;
        }
    }
    void setParams(Vector2 pos, Vector2 vel)
    {
        obj.setPos(pos);
        obj.applyImpulse(Vector2Scale(vel, obj.mass));
    }
    void update()
    {
        if(active)
        {
            obj.update();
            lifetime -= 1/fps;
            if(lifetime < 0)
                active = false;
        }
    }
    void draw()
    {
        if(active)
            DrawCircleV(obj.getPos(), 3, MAGENTA);
        // DrawText(std::to_string(lifetime).c_str(), obj.getPos().x, obj.getPos().y, 10, WHITE);
        // DrawText(std::to_string(active).c_str(), obj.getPos().x, obj.getPos().y + 15, 10, WHITE);
    }
};
class Turret
{
    public:
    float heading = 0;
    float targeting = 0;

};

class Ship // Spaceship, controlled by the player and AI (AI sold separately)
{
private:
    bool initialized = false;
    float baseThrust = 1500, baseAngleAccel = 10000;
    float thrust = baseThrust, angleAccel = baseAngleAccel;
    float maxEnergy, energy, baseRechargeRate = 5, rechargeRate = 5;
    float maxHP, HP;
public:
    Layout layout;
    Object *obj;
    size_t id;
    Ship()
    {
        maxEnergy = 0;
        energy = 0;
        maxHP = 0;
        HP = 0;
        obj = nullptr;
        id = 0;
    }
    void init(Object* objPtr)
    {
        obj = objPtr;
        obj->initDefault();

        obj->setPos((Vector2){0, 28});
        obj->setMass(10);
        // obj->initHitboxFromArray(shipVertices, shipIndices, 6, 4);
        obj->initHitboxFromFile("data/viper.box");

        maxHP = 100;

        maxEnergy = 10;
        HP = maxHP;
        energy = maxEnergy;

        layout.initDefault();

    }
    void turnToMouse(Vector2 mousePos, bool key)
    {
        float stopAngle = fabs(obj->getAngleVel())*obj->getAngleVel() / 2 / (angleAccel/obj->mass/obj->momentInertia);
        bool shouldStop = fabs(stopAngle) > PI;
        stopAngle += obj->getDirection();
        
        if(key)
        {
            
            Vector2 relVec = Vector2Subtract(mousePos, obj->getPos());
            relVec = Vector2Normalize(relVec);
            float relAngle = atan2(relVec.y, relVec.x);
            bool direction = (relAngle - stopAngle > 0);
            if(fabs(relAngle - stopAngle) > PI)
                direction = !direction;

            if(!shouldStop)
                obj->applyMoment(angleAccel * sgn<float>(direction*2 - 1));
            else
                obj->applyMoment(angleAccel * -sgn<float>(obj->getAngleVel()));

        
        }
    }
    void move(bool wkey, bool skey, bool akey, bool dkey)
    {
        if(wkey)
        {
            obj->applyForce(Vector2Rotate((Vector2){thrust, 0}, obj->getDirection()));
            energy -= 6/fps;
        }
        energy += baseRechargeRate/fps;
        energy = Clamp(energy, 0, maxEnergy);
    }
    void fire(Projectile* projectilePtr, size_t type)
    {
        switch (type)
        {
        case 1:
            projectilePtr->init(Projectile::Behavior::KINETIC);
            projectilePtr->lifetime = 1;
            projectilePtr->spawnedBy = id;

            Vector2 vel = Vector2Add(obj->getVel(), Vector2Rotate((Vector2){200, 0}, obj->getDirection()));
            projectilePtr->obj.applyImpulse(Vector2Scale(vel, projectilePtr->obj.mass));
            obj->applyImpulse(Vector2Scale(vel, -projectilePtr->obj.mass));
            projectilePtr->obj.setPos(obj->getPos());
        break;
        }
    }
    void hurt(float damage)
    {
        HP -= damage;
    }

    void draw()
    {
        obj->draw();
        
    }
    void drawLayout()
    {
        obj->draw(6, (Color){25, 25, 100, 255});
        Vector2 offset = layout.offset;
        for(int i=0; i<layout.lines; i++)
        {
            for(int j=0; j<layout.columns; j++)
            {
                char x = layout.layoutArray[i][j];
                
                Color drawColor = WHITE;
                switch (x)
                {
                case 'r':
                    drawColor = RED;
                    break;  
                case 'g':
                    drawColor = GREEN;
                    break;
                case 'b':
                    drawColor = BLUE;
                    break;
                case 'y':
                    drawColor = YELLOW;
                    break;
                default:
                    drawColor = (Color){0, 0, 0, 0};
                    break;
                }
                DrawRectangleV((Vector2){offset.x + j*30, offset.y + i*30}, (Vector2){25, 25}, drawColor);
                if(layout.occupied[i*layout.columns + j])
                    DrawRectangleV((Vector2){offset.x + j*30, offset.y + i*30}, (Vector2){5, 5}, RAYWHITE);
            }
        }
        for(int i=0; i<layout.componentArray.size(); i++)
        {
            Vector2 drawPos = (Vector2){layout.componentArray[i].column*30 + offset.x,
                                        layout.componentArray[i].line*30 + offset.y};
            DrawRectangleV((Vector2){drawPos.x + 5, drawPos.y}, (Vector2){5, 5}, MAGENTA);
        }
    }
    Object* getObjPtr()
    {
        return obj;
    }
    Vector2 getPos()
    {
        return obj->getPos();
    }
    Vector3 getVitals()
    {
        return (Vector3){energy/maxEnergy, HP/maxHP, 0};
    }
    float getAngle()
    {
        return obj->getDirection();
    }

};