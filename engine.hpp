// My 2D physics engine
#include <iostream>
#include <cmath>
#include <fstream>

#ifndef RAYLIB_H
#include <raylib.h>
#define RAYLIB_H
#endif

#ifndef RAYMATH_H
#include <raymath.h>
#define RAYMATH_H
#endif

const float fps = 60;
const int physicsTicks = 1;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

inline float toDegrees(int angle)
{
    return angle * 180 / PI;
}

inline float crossProduct2D(Vector2 v1, Vector2 v2)
{
    return v1.x * v2.y - v2.x * v1.y;
}

float wrapAngle(float angle)
{
    if(angle > PI)
        angle -= 2*PI;
    if(angle < -PI)
        angle += 2*PI;

    return angle;
}

class Hitbox // object's hitbox in local coordinates
{
public:
    bool initialized = false;
    int vertexCount, indexCount;
    Vector2* vertices;
    int* indices;

    enum Type
    {
        POINT, LINE, CIRCLE, TRIANGLE
    };

    Type type;

    Vector2 topleft, bottomright; // axis aligned bounding box
    Vector2 collisionNormal;

    Hitbox()
    {
        
    }
    void initDefault()
    {
        vertexCount = 3;
        indexCount = 3;
        vertices = new Vector2[3] {(Vector2){20, 0}, (Vector2){-20, -15}, (Vector2){-20, 15} };

        indices = new int[indexCount];
        for(int i=0; i<3; i++)
        {
            indices[i] = i;

        }
        type = Type::TRIANGLE;
        initialized = true;
    }
    void initFromArray(const Vector2* vertexArray, const int* indexArray, const int vertexNum, const int triangles)
    {
        type = Type::TRIANGLE;

        vertexCount = vertexNum;
        indexCount = triangles * 3;
        vertices = new Vector2[vertexNum];
        indices = new int[triangles];

        for(int i=0; i<vertexNum; i++)
        {
            vertices[i] = vertexArray[i];

            topleft.x = std::min(topleft.x, vertices[i].x);
            topleft.y = std::min(topleft.y, vertices[i].y);
            bottomright.x = std::max(bottomright.x, vertices[i].x);
            bottomright.y = std::max(bottomright.y, vertices[i].y);
        }
        for(int i=0; i<triangles * 3; i++)
        {
            indices[i] = indexArray[i];
        }
        initialized = true;

    }
    void initFromFile(const std::string filename)
    {
        type = Type::TRIANGLE;

        std::fstream file;
        file.open(filename);
        file >> vertexCount >> indexCount;
        indexCount *= 3;

        vertices = new Vector2[vertexCount];
        indices = new int[indexCount];

        for(int i=0; i<vertexCount; i++)
        {
            file >> vertices[i].x >> vertices[i].y;
            std::cout << vertices[i].x << " " << vertices[i].y << std::endl;

        }
        for(int i=0; i<indexCount; i++)
        {
            file >> indices[i];
            std::cout << indices[i] << std::endl;
        }
        initialized = true;    
    }

    void initPoint()
    {
        type = Type::POINT;
        initialized = true;
    }

    bool contains(Vector2 p)
    {
        bool flag;
        for(int i=0; i<indexCount; i+=3)
        {
            flag = true;
            collisionNormal = (Vector2){0, 0};
            float minDepth = 1000;
            for(int j=0; j<3; j++)
            {
                Vector2 relVec = Vector2Subtract(p, vertices[indices[i+j]]);
                Vector2 edgeVec = Vector2Normalize(Vector2Subtract(vertices[indices[i + (j+1)%3]], vertices[indices[i + j]] ));

                minDepth = std::min(crossProduct2D(relVec, edgeVec), minDepth);
                
                if(crossProduct2D(relVec, edgeVec) < 0)
                {
                    flag = false;
                    break;
                }
                else
                {
                    if(fabs(crossProduct2D(relVec, edgeVec) - minDepth) < 0.001f)
                    {
                        collisionNormal = Vector2Scale(Vector2Rotate(edgeVec, PI/2), minDepth);
                    }
                }
            }
            if(flag)
                return true;
            
        }
        return false;
    }
    ~Hitbox()
    {
        // if(type == Type::TRIANGLE)
        // {
        //     delete[](vertices);
        //     delete[](indices);
        // }
    }

};

class Object // physical object, for which all physics calculations are applied
{
private:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;

    Hitbox hbox;

    float direction;
    float angleVel, angleAccel; // all in radians/sec

    float rebound;

    Color drawColor;
public:

    float mass, momentInertia;
    Object()
    {
        
    }
    void initDefault()
    {
        position = (Vector2){0, 0};
        velocity = (Vector2){0, 0};
        acceleration = (Vector2){0, 0};

        direction = 0;
        angleVel = 0;
        angleAccel = 0;

        mass = 10;
        momentInertia = 750;

        rebound = 0.0f;

        drawColor = GREEN;
    }

    bool initHitboxFromArray(const Vector2* vertexArray, const int* indexArray, const int vertexNum, const int triangles)
    {
        hbox.initFromArray(vertexArray, indexArray, vertexNum, triangles);
        return true;
    }

    bool initHitboxFromFile(const std::string filename)
    {
        hbox.initFromFile(filename);
        return true;
    }

    bool initHitboxDefault()
    {
        hbox.initDefault();
        return true;
    }

    bool initHitboxPoint()
    {
        hbox.initPoint();
        return true;
    }

    void update()
    {
        velocity = Vector2Add(velocity, Vector2Scale(acceleration, 1/fps/physicsTicks));
        position = Vector2Add(position, Vector2Scale(velocity, 1/fps/physicsTicks));

        angleVel += angleAccel * 1/fps/physicsTicks;
        direction += angleVel * 1/fps/physicsTicks;

        acceleration = (Vector2){0, 0};
        angleAccel = 0;

        direction = wrapAngle(direction);

    }
    inline Vector2 toLocal(Vector2 p, Vector2 pos, float rotation)
    {
        p = Vector2Add(Vector2Rotate(p, rotation), pos);
        return Vector2Rotate(Vector2Subtract(p, position), -direction);
    }
    inline Vector2 toGlobal(Vector2 p)
    {
        return Vector2Add(position, Vector2Rotate(p, direction));
    }

    void setPos(Vector2 pos)
    {
        position = pos;
    }
    void setMass(float mass)
    {
        this->mass = mass;
    }

    void applyForce(Vector2 force)
    {
        acceleration = Vector2Add(acceleration, Vector2Scale(force, 1 / mass));
    }
    void applyMoment(float moment)
    {
        angleAccel += moment / mass / momentInertia;
    }
    void applyImpulse(Vector2 impulse)
    {
        velocity = Vector2Add(velocity, Vector2Scale(impulse, 1/mass));
    }
    void applyRotationImpulse(float imp)
    {
        angleVel += imp/(momentInertia*mass);
    }

    void setAngle(float angle)
    {
        direction = Clamp(angle, -PI, PI);
    }

    Vector2 getPos()
    {
        return position;
    }
    Vector2 getVel()
    {
        return velocity;
    }

    float getDirection()
    {
        return direction;
    }
    float getAngleVel()
    {
        return angleVel;
    }
    void setAngleVel(float angleSpd)
    {
        angleVel = angleSpd;
    }

    bool checkCollision(Object* obj)
    {
        Vector2 collisionNormal;
        Vector2 collisionPoint; // common point of two objects in global coords

        int collisionCase = obj->hbox.type * 10 + hbox.type;

        bool ans = false;
        switch(collisionCase)
        {
        case 33:
            for(int i=0; i<obj->hbox.vertexCount; i++)
            {
                if(hbox.contains(toLocal(obj->hbox.vertices[i], obj->position, obj->direction))) // if a vertex of obj is inside this
                {
                    drawColor = RED;

                    collisionNormal = Vector2Rotate(hbox.collisionNormal, direction);
                    collisionPoint = obj->toGlobal(obj->hbox.vertices[i]);

                    handleCollision(obj, collisionNormal, collisionPoint, true);
                    ans = true;
                }
            }
        break;
        case 3:
            if(hbox.contains(toLocal(Vector2Zero(), obj->position, obj->direction)))
            {
                collisionNormal = Vector2Rotate(hbox.collisionNormal, direction);
                collisionPoint = obj->position;

                handleCollision(obj, collisionNormal, collisionPoint, true);
                ans = true;
            }
            break;
        }
        drawColor = BLUE;
        return ans;
    }

    bool handleCollision(Object* obj, Vector2 collisionNormal, Vector2 collisionPoint, bool thisNormal) // collision normal in global coords
    {
        float kRebound = Clamp((rebound + obj->rebound)/2, 0, 1) + 1;
        float direction = thisNormal*2-1;

        float k = obj->mass / (mass + obj->mass);
        position = Vector2Subtract(position, Vector2Scale(collisionNormal, k*direction));
        obj->position = Vector2Subtract(obj->position, Vector2Scale(collisionNormal, (k-1)*direction));

        // float depth = Vector2Length(collisionNormal);
        collisionNormal = Vector2Normalize(collisionNormal);

        // calculating collision point relative to this and obj
        Vector2 relPos1 = Vector2Subtract(collisionPoint, position);
        Vector2 relPos2 = Vector2Subtract(collisionPoint, obj->position);

        // finding the velocity of system's center of mass
        Vector2 velCenterMass = Vector2Add(Vector2Scale(velocity, mass), Vector2Scale(obj->velocity, obj->mass));
        velCenterMass = Vector2Scale(velCenterMass, 1/(mass + obj->mass));

        // calculating v1 and v2 of colliding objects relative to their center of mass
        Vector2 relVel1 = Vector2Subtract(velocity, velCenterMass);
        Vector2 relVel2 = Vector2Subtract(obj->velocity, velCenterMass);

        // add rotational velocity of colliding point
        relVel1 = Vector2Add(relVel1, Vector2Scale(Vector2Rotate(relPos1, PI/2), angleVel));
        relVel2 = Vector2Add(relVel2, Vector2Scale(Vector2Rotate(relPos2, PI/2), obj->angleVel));


        float lever1 = fabs(crossProduct2D(relPos1, collisionNormal));
        float lever2 = fabs(crossProduct2D(relPos2, collisionNormal));

        float coeff =   1/mass*(1 + lever1/momentInertia) + 
                        1/obj->mass*(1 + lever2/obj->momentInertia);
        coeff = (Vector2DotProduct(Vector2Subtract(relVel1, relVel2), collisionNormal)) / coeff;

        Vector2 imp = Vector2Scale(collisionNormal, -coeff * kRebound);


        applyImpulse(imp);
        obj->applyImpulse(Vector2Scale(imp, -1));

        applyRotationImpulse(crossProduct2D(relPos1, imp));
        obj->applyRotationImpulse(-crossProduct2D(relPos2, imp));

        return true;
    }

    void draw()
    {

        for(int i=0; i < hbox.indexCount; i += 3)
        {

            Vector2 v1, v2, v3;
            v1 = Vector2Add(Vector2Rotate(hbox.vertices[hbox.indices[i]], direction), position);
            v2 = Vector2Add(Vector2Rotate(hbox.vertices[hbox.indices[i + 1]], direction), position);
            v3 = Vector2Add(Vector2Rotate(hbox.vertices[hbox.indices[i + 2]], direction), position);

            DrawTriangle(v1, v2, v3, drawColor);
        }
    }
    void draw(float scale, Color col)
    {
        for(int i=0; i < hbox.indexCount; i += 3)
        {

            Vector2 v1, v2, v3;
            v1 = Vector2Scale(hbox.vertices[hbox.indices[i]], scale);
            v2 = Vector2Scale(hbox.vertices[hbox.indices[i+1]], scale);
            v3 = Vector2Scale(hbox.vertices[hbox.indices[i+2]], scale);

            v1 = Vector2Add(Vector2Rotate(v1, direction), position);
            v2 = Vector2Add(Vector2Rotate(v2, direction), position);
            v3 = Vector2Add(Vector2Rotate(v3, direction), position);

            DrawTriangle(v1, v2, v3, col);
        }
    }

};

