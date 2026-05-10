#pragma once
#ifndef FastEnemy_H
#define FastEnemy_H
#include "Enemy.h"

class fastenemy : public Enemy {
private:
    CircleShape shape;
public:
    fastenemy(float x, float y, Vector2f* p, int pL) : Enemy(x, y, 80,80.f, 15, p, pL, 65.f, 2, 0.8f) 
    {
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color(2, 50, 50));
        shape.setOrigin(10.f, 10.f);
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
        drawHPBar(window, 24.f, 20.f);
    }
};

#endif