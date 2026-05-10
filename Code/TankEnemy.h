#pragma once
#ifndef TankEnemy_H
#define TankEnemy_H
#include "Enemy.h"

class tankenemy : public Enemy {
private:
    sf::CircleShape shape;
public:
    tankenemy(float x, float y, Vector2f* p, int pL) : Enemy(x, y, 300, 35.f,20, p, pL,70.f,15, 2.0f) 
    {
        shape.setRadius(14.f);
        shape.setFillColor(Color(78, 60, 5));
        shape.setOrigin(14.f, 14.f);
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
        drawHPBar(window, 32.f, 28.f);
    }
};

#endif