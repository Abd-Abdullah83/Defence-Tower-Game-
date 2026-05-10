#pragma once
#ifndef Bullet_H
#define Bullet_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

class Bullet {
private:
    float x, y;
    float vx, vy;
    int   damage;
    bool  active;
    bool  fromEnemy;   // true => hurts towers; false => hurts enemies
    sf::CircleShape shape;

public:
    Bullet(float startX, float startY,
        float targetX, float targetY,
        int dmg, bool enemyBullet,
        sf::Color color, float speed = 300.f)
        : x(startX), y(startY),
        damage(dmg), active(true), fromEnemy(enemyBullet)
    {
        float dx = targetX - startX;
        float dy = targetY - startY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0.001f) {
            vx = (dx / dist) * speed;
            vy = (dy / dist) * speed;
        }
        else {
            vx = speed; vy = 0.f;
        }

        float r = enemyBullet ? 5.f : 4.f;
        shape.setRadius(r);
        shape.setOrigin(r, r);
        shape.setFillColor(color);
    }

    void update(float dt) {
        if (!active) return;
        x += vx * dt;
        y += vy * dt;
        // deactivate when off-screen
        if (x < -20.f || x > 1020.f || y < -20.f || y > 620.f)
            active = false;
    }

    void render(sf::RenderWindow& window) {
        if (!active) return;
        shape.setPosition(x, y);
        window.draw(shape);
    }

    bool  isActive()    const { return active; }
    void  deactivate() { active = false; }
    float getX()        const { return x; }
    float getY()        const { return y; }
    int   getDamage()   const { return damage; }
    bool  isFromEnemy() const { return fromEnemy; }
};

#endif