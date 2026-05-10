#pragma once
#ifndef Enemy_H
#define Enemy_H
#include "Entity.h"
using namespace sf;
#include <cmath>

class Enemy : public Entity {
protected:
    float spd, baseSpd;
    int   reward, pathIndex, pathLength;
    sf::Vector2f* path;      // pointer to map path (not owned)

    // --- slow effect ---
    float slowTimer;         // counts down; resets speed when it hits 0

    // --- attacking towers ---
    float attackRange;
    int   attackDamage;
    float attackRate;        // seconds between shots
    float attackCooldown;    // current countdown
    bool  attacking;         // set each frame by Game; if true, don't move

public:
    Enemy(float a, float b, int h, float s, int rd,sf::Vector2f* p, int pL, float range = 60.f, int Dmg = 5, float rate = 1.5f) : Entity(a, b, h)
    {
        spd = baseSpd = s;
        pathIndex = 0, slowTimer = 0, attacking = false, attackCooldown = 0;
        attackDamage = Dmg, attackRange = range, attackRate = rate;
        reward = rd, path = p, pathLength = pL;
    }

   


    void slow(float factor, float t = 3.f)// t tell time duration for slow down enemy
    {
        spd = baseSpd * factor;
        slowTimer = t;
    }
    void render(sf::RenderWindow& window) = 0;
    virtual void move(float dt) = 0;

    void update(float dt) 
    {
        // tick attack cooldown
        if (attackCooldown > 0.f) attackCooldown -= dt;
        // tick slow effect
        if (slowTimer > 0.f) {
            slowTimer -= dt;
            if (slowTimer <= 0.f) spd = baseSpd;   // wear off
        }
        // only walk when not engaged with a tower
        if (!attacking) move(dt);
    }
    bool  atend() const 
    {
        return pathIndex >= pathLength;
    }
    int   getReward() const
    { 
        return reward;
    }
    float getSpeed() const 
    { 
        return spd;
    }
    float getAttackRange() const 
    {
        return attackRange; 
    }
    int   getAttackDamage() const
    { 
        return attackDamage;
    }
    bool  canAttack() const 
    {
        return attackCooldown <= 0.f; 
    }
    void  resetAttackCooldown()
    {
        attackCooldown = attackRate;
    }
    void  setAttacking(bool v) 
    {
        attacking = v;
    }
    bool  isAttackingTower()  const // it can be used to find whether emeny is moving or firing
    {
        return attacking; 
    }
    virtual ~Enemy() {}
protected:
    void drawHPBar(sf::RenderWindow& window, float width, float height) {
        sf::RectangleShape bg(sf::Vector2f(width, 5.f));
        bg.setFillColor(sf::Color::Red);
        bg.setPosition(x - width / 2.f, y - height / 2.f - 8.f);

        float ratio = (float)hp / (float)maxhp;
        sf::RectangleShape fg(sf::Vector2f(width * ratio, 5.f));
        fg.setFillColor(sf::Color::Green);
        fg.setPosition(x - width / 2.f, y - height / 2.f - 8.f);

        window.draw(bg);
        window.draw(fg);
    }
};

#endif