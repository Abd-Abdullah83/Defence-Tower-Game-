#pragma once
#ifndef FlyingEnemy_H
#define FlyingEnemy_H
#include "Enemy.h"

class flyingenemy : public Enemy {
private:
    CircleShape shape;
    Vector2f FT;
public:
    flyingenemy(float x, float y, Vector2f* p, int pL) : Enemy(x, y,160, 90.f,20, p, pL,55.f, 8, 1.2f)
    {
        FT = path[pathLength - 1];
        shape.setRadius(10.f);
        shape.setFillColor(Color(25, 250, 20));
        shape.setOrigin(10.f, 10.f);
    }
    void move(float dt)  {
        float dx, dy, dis;
        dx = FT.x - x;
        dy = FT.y - y;
        dis = sqrt(dx * dx + dy * dy);
        if (dis < 2.f) 
        {
            pathIndex = pathLength;  
            return;
        }
        x += (dx / dis) * spd * dt;
        y += (dy / dis) * spd * dt;
    }
    void render(RenderWindow& window)
    {
        shape.setPosition(x, y);
        window.draw(shape);
        drawHPBar(window, 28.f, 24.f);
    }
};

#endif