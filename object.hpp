#ifndef ENGINE
#include "engine.hpp"
#define ENGINE
#endif

// #include <vector>

const int window_width = 1280, window_height = 720;

float sfabs(float val, float eps) // fabs safe from division by zero
{
    return fabs(val) + eps + EPSILON;
}

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
                        {-14, 12},
                        {4, -5},
                        {-14, -12}
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
        //UpdateCameraPro(&cam, Vector3Zero(), Vector3Zero(), Clamp(scale, minScale, maxScale));
        float xscale = window_width / 2 / softScale(farthestCoords.x, 100);
        float yscale = window_height / 2 / softScale(farthestCoords.y, 100);
        float zoom = std::min(xscale, yscale);
        cam.zoom = Clamp(zoom, minScale, maxScale);
        //cam.target = Vector2Scale(farthestCoords, 1);
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
        return Vector2Scale(mouseCoords, 1/cam.zoom);
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

        enemyEnergyBar.width = enemyVitals.x*width;
        //enemyEnergyBar.x = (1-enemyVitals.x)*width;

        DrawMyRect(allyEnergyBar, YELLOW);
        DrawMyRect(allyHPBar, GREEN);

        DrawMyRect(enemyEnergyBar, YELLOW);
        DrawMyRect(enemyHPBar, GREEN);
    }
};

class Layout
{
private:
    int state=0;
public:
    unsigned char* layoutArray;
    
};

class Projectile
{
private:
    Object obj;
    enum Behavior
    {
        KINETIC, BEAM
    };
    Behavior type;

    float lifetime;
public:
    Projectile()
    {
        obj.initHitboxPoint();
        type = KINETIC;
    }
};

class Ship // Spaceship, controlled by the player and AI (sold separately)
{
private:
    float baseThrust = 1500, baseAngleAccel = 5000;
    float thrust = baseThrust, angleAccel = baseAngleAccel;
    float maxEnergy, energy, baseRechargeRate = 5, rechargeRate = 5;
    float maxHP, HP;
public:
    Object obj;
    Ship()
    {
        obj.setPos((Vector2){0, 28});
        obj.setMass(10);
        obj.initHitboxFromArray(shipVertices, shipIndices, 6, 4);

        maxHP = 100;
        maxEnergy = 10;
        HP = maxHP;
        energy = maxEnergy;
    }
    void turnToMouse(Vector2 mousePos, bool key)
    {
        float stopAngle = fabs(obj.getAngleVel())*obj.getAngleVel() / 2 / (angleAccel/obj.mass/obj.momentInertia);
        bool shouldStop = fabs(stopAngle) > PI;
        stopAngle += obj.getDirection();
        
        if(key)
        {
            
            Vector2 relVec = Vector2Subtract(mousePos, obj.getPos());
            relVec = Vector2Normalize(relVec);
            float relAngle = atan2(relVec.y, relVec.x);
            bool direction = (relAngle - stopAngle > 0);
            if(fabs(relAngle - stopAngle) > PI)
                direction = !direction;

            if(!shouldStop)
                obj.applyMoment(angleAccel * sgn<float>(direction*2 - 1));
            else
                obj.applyMoment(angleAccel * -sgn<float>(obj.getAngleVel()));

        
        }
    }
    void move(bool wkey, bool skey, bool akey, bool dkey)
    {
        if(wkey)
        {
            obj.applyForce(Vector2Rotate((Vector2){thrust, 0}, obj.getDirection()));
            energy -= 6/fps;
        }
        energy += baseRechargeRate/fps;
        energy = Clamp(energy, 0, maxEnergy);
        // if(akey)
        //     obj.applyMoment(-defAngleAccel);
        // if(dkey)
        //     obj.applyMoment(defAngleAccel);

        obj.update();
    }
    void draw()
    {
        obj.draw();
    }
    Object getObj()
    {
        return obj;
    }
    Vector2 getPos()
    {
        return obj.getPos();
    }
    Vector2 getSize()
    {
        return obj.getBounds();
    }
    Vector2 getOrigin()
    {
        return obj.getOrigin();
    }
    Vector3 getVitals()
    {
        return (Vector3){energy/maxEnergy, HP/maxHP, 0};
    }
    float getAngle()
    {
        return obj.getDirection();
    }

};