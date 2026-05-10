#pragma once
#ifndef SlowTower_H
#define SlowTower_H
#include "Tower.h"

class slowtower : public Tower {
private:
    CircleShape  aura;   // visual pulse ring
    RectangleShape base;
    float pulseAlpha,pulseDir;
public:
    slowtower(float x, float y)
        : Tower(x, y, 120.f, 0.f, 1.5f, 200),
        pulseAlpha(30.f), pulseDir(1.f)
    {
        base.setSize(sf::Vector2f(36.f, 36.f));
        base.setOrigin(18.f, 18.f);
        base.setFillColor(sf::Color(60, 100, 180));
        base.setOutlineColor(sf::Color(40, 40, 40));
        base.setOutlineThickness(2.f);

        aura.setRadius(range);
        aura.setOrigin(range, range);
        aura.setFillColor(sf::Color::Transparent);
        aura.setOutlineThickness(2.f);
    }
    Bullet* attack(Enemy** e, int n)
    {
        if (!canFire())
            return nullptr;
        for (int i = 0; i < n; i++) {
            if (!e[i]->isAlive()) continue;
            float dx = e[i]->getX() - x;
            float dy = e[i]->getY() - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist <= range)
                e[i]->slow(0.45f, 3.f);   // 45% speed for 3 s
        }
        fcooldown = fRate;
        return nullptr;
    }

    void update(float dt) override {
        Tower::update(dt);
        // animate aura pulse
        pulseAlpha += pulseDir * 60.f * dt;
        if (pulseAlpha > 80.f) { pulseAlpha = 80.f; pulseDir = -1.f; }
        if (pulseAlpha < 10.f) { pulseAlpha = 10.f; pulseDir = 1.f; }
    }

    void render(sf::RenderWindow& window) override {
        // animated aura
        aura.setPosition(x, y);
        aura.setOutlineColor(sf::Color(80, 160, 255, (sf::Uint8)pulseAlpha));
        window.draw(aura);

        base.setPosition(x, y);
        window.draw(base);
        drawTowerHP(window);
    }
};

#endif