#pragma once
#ifndef Sniper_H
#define Sniper_H
#include "Tower.h"

class snipertower : public Tower {
private:
    RectangleShape base, barrel;
    float angle;
public:
    snipertower(float x, float y) : Tower(x, y, 200.f, 80.f, 2.0f, 120)
    {
        angle = 0.f;
        upgradeDamageFactor = 1.6f;
        upgradeRangeFactor = 1.3f;
        base.setSize(Vector2f(28.f, 36.f)), base.setOrigin(14.f, 18.f);
        base.setFillColor(Color(60, 180, 220)),base.setOutlineColor(Color(40, 40, 40));
        base.setOutlineThickness(2.f);
        upgradeCost = cost*2/3;
        barrel.setSize(Vector2f(30.f, 4.f)),barrel.setOrigin(0.f, 2.f);
        barrel.setFillColor(Color(5, 50, 50));
    }
    Bullet* attack(Enemy** e, int n) 
    {
        if (!canFire())
            return nullptr;
        Enemy* target = findTarget(e, n);
        if (!target) 
            return nullptr;
        float dx, dy;
        fcooldown = fRate;
        dx = target->getX() - x,dy = target->getY() - y;
        const float PI = 3.14159265f;
        angle = atan2(dy, dx) * 180.f / PI;
        return new Bullet(x, y,target->getX(), target->getY(),(int)getEffectiveDamage(), false, Color(255, 200, 50), 280.f);               
    }
    void update(float dt) 
    {
        Tower::update(dt);
    }
    void render(sf::RenderWindow& window) 
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