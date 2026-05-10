#pragma once
#ifndef Machine_H
#define Machine_H
#include "Tower.h"

class machineguntower : public Tower {
private:
    RectangleShape base,barrel;
    float angle;
public:
    machineguntower(float x, float y) : Tower(x, y, 100.f, 20.f, 0.2f, 180)
    {
        angle = 0.f, upgradeCost = cost * 2 / 3;
        upgradeDamageFactor = 1.4f;
        upgradeRangeFactor = 1.1f;
        base.setSize(Vector2f(36.f, 36.f)),base.setOrigin(18.f, 18.f);
        base.setFillColor(Color(160, 115, 180)), base.setOutlineColor(Color(40, 40, 40));
        base.setOutlineThickness(2.f);
        barrel.setSize(Vector2f(22.f, 5.f)),barrel.setOrigin(0.f, 2.5f);
        barrel.setFillColor(Color(50, 50, 50));
    }
    Bullet* attack(Enemy** e, int n) 
    {
        if (!canFire())
            return nullptr;
        Enemy* target = findTarget(e, n);
        if (!target)
            return nullptr;
        fcooldown = fRate;
        float dx, dy;
        dx = target->getX() - x;
        dy = target->getY() - y;
        const float PI = 3.14159265f;
        angle = std::atan2(dy, dx) * 180.f / PI;
        return new Bullet(x, y,target->getX(), target->getY(), (int)getEffectiveDamage(), false, Color(255, 200, 50), 280.f);
    }
    void update(float dt)
    {
        Tower::update(dt);
    }
    void render(RenderWindow& window)
    {
        drawRangeCircle(window);
        base.setPosition(x, y);
        window.draw(base);
        barrel.setPosition(x, y);
        barrel.setRotation(angle);
        window.draw(barrel);
        drawTowerHP(window);
    }
};

#endif