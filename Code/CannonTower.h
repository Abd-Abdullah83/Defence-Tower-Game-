#pragma once
#ifndef Cannon_H
#define Cannon_H
#include "Tower.h"

class CannonTower : public Tower {
private:
    RectangleShape base,barrel;
    float angle;
public:
    CannonTower(float x, float y) : Tower(x, y, 120.f, 50.f, 1.0f, 100)
    {
        angle = 0.f, upgradeCost = cost * 2 / 3;
        upgradeDamageFactor = 1.4f;
        upgradeRangeFactor = 1.15f;
        base.setSize(sf::Vector2f(36.f, 36.f)), base.setOrigin(18.f, 18.f);
        base.setFillColor(sf::Color(200, 80, 60)), base.setOutlineColor(Color(40, 40, 40)),base.setOutlineThickness(2.f);
        barrel.setSize(sf::Vector2f(22.f, 6.f)), barrel.setOrigin(0.f, 3.f);
        barrel.setFillColor(sf::Color(50, 50, 50));
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
        angle = atan2(dy, dx) * 180.f / PI;
        return new Bullet(x, y, target->getX(), target->getY(), (int)getEffectiveDamage(), false, Color(255, 200, 50), 280.f);
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