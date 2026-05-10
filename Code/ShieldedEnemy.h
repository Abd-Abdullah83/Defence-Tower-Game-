#pragma once
#ifndef ShieldedEnemy_H
#define ShieldedEnemy_H
#include "Enemy.h"

class ShieldedEnemy : public Enemy {
private:
    int NSH; //Number of Hits to destroy shield
    CircleShape shape,shield;
public:
    ShieldedEnemy(float x, float y, Vector2f* p, int pL) : Enemy(x, y,120,50.f,25, p, pL,60.f,6, 1.8f)
    {
        NSH = 3;
        shape.setRadius(13.f), shield.setRadius(16.f);
        shape.setFillColor(Color(70, 70, 220)), shield.setFillColor(Color(100, 100, 255, 80));
        shape.setOrigin(13.f, 13.f),shield.setOrigin(16.f, 16.f);
    }
    void takeDamage(int amount) {
        if (NSH > 0) {
            NSH--;
            return;
        }
        Entity::takeDamage(amount);
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
        if (NSH > 0) 
        {
            shield.setPosition(x, y);
            window.draw(shield);
        }
        shape.setPosition(x, y);
        window.draw(shape);
        drawHPBar(window, 30.f, 26.f);
    }
};

#endif