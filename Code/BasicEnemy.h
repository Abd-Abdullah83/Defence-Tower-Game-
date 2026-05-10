#pragma once
#ifndef BasicEnemy_H
#define BasicEnemy_H
#include "Enemy.h"

class BasicEnemy : public Enemy {
private:
    CircleShape shape;
public:
    BasicEnemy(float x, float y, Vector2f* p, int pL) : Enemy(x, y, 100, 60.f,10, p, pL, 60.f, 4,1.5f)   
    {
        shape.setRadius(12.f);
        shape.setFillColor(Color(220, 50, 50));
        shape.setOrigin(12.f, 12.f);
    }
    void move(float dt) 
    {
        if (pathIndex >= pathLength) 
            return;
       Vector2f target = path[pathIndex];
        float dx, dy, dis;
        dx = target.x - x;
        dy = target.y - y;
        dis = sqrt(dx * dx + dy * dy);
        if (dis < 2.f) 
            pathIndex++;
        else 
        {
            x += (dx / dis) * spd * dt;
            y += (dy / dis) * spd * dt;
        }
    }
    void render(RenderWindow& window) 
    {
        shape.setPosition(x, y);
        window.draw(shape);
        drawHPBar(window, 28.f, 24.f);
    }
};

#endif