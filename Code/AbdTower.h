#pragma once
#ifndef AbdTower_H
#define AbdTower_H
#include "Tower.h"
#include <cmath>

class AbdTower : public Tower {
private:
    CircleShape aura;
    RectangleShape base;
    float pulseAlpha, pulseDir, damageBoost, speedBoost;     

public:
    AbdTower(float x, float y) : Tower(x, y, 90.f, 0.f, 2.0f, 150)
    {
        pulseAlpha = 30.f, pulseDir = 1.f;
        damageBoost = 1.5f, speedBoost = 1.0f;
        base.setSize(sf::Vector2f(36.f, 36.f));
        base.setOrigin(18.f, 18.f),base.setFillColor(Color(100, 200, 100));    
        base.setOutlineColor(Color(40, 40, 40)),base.setOutlineThickness(2.f);
        aura.setRadius(range), aura.setOrigin(range, range);
        aura.setFillColor(Color::Transparent),aura.setOutlineThickness(2.f);
    }
    Bullet* attack(Enemy** e, int n)
    {
        if (!canFire()) 
            return nullptr;
        fcooldown = fRate;
        return nullptr;  
    }

    void applyBuffTo(Tower* t) {
        if (!t || !t->isAlive()) return;
        float dx = t->getX() - x;
        float dy = t->getY() - y;
        if (std::sqrt(dx * dx + dy * dy) <= range) {
            t->setBuffs(damageBoost, speedBoost);
        }
    }

    void update(float dt) override {
        Tower::update(dt);

        pulseAlpha += pulseDir * 60.f * dt;
        if (pulseAlpha > 80.f) { pulseAlpha = 80.f; pulseDir = -1.f; }
        if (pulseAlpha < 10.f) { pulseAlpha = 10.f; pulseDir = 1.f; }
    }

    void render(sf::RenderWindow& window) override {
        aura.setPosition(x, y);
        aura.setOutlineColor(sf::Color(100, 220, 100, (sf::Uint8)pulseAlpha));
        window.draw(aura);
        base.setPosition(x, y);
        window.draw(base);
        drawTowerHP(window);
    }
};

#endif