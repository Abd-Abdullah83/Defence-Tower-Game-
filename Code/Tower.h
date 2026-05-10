#pragma once
#ifndef Tower_H
#define Tower_H
#include "Entity.h"
#include "Enemy.h"
#include "Bullet.h"
#include <cmath>

const int MAX_ENEMIES = 100;

class Tower : public Entity {
protected:
    float range, damage, fRate, fcooldown, dmgMult, fRateMult, upgradeDamageFactor, upgradeRangeFactor;
    int   cost, level, maxLevel, upgradeCost; 
    void drawRangeCircle(RenderWindow& win)
    {
        sf::CircleShape rc(range);
        rc.setOrigin(range, range), rc.setPosition(x, y);
        rc.setFillColor(Color(255, 255, 255, 15));
        rc.setOutlineColor(Color(255, 255, 255, 50)), rc.setOutlineThickness(1.f);

        win.draw(rc);
    }
    void drawTowerHP(RenderWindow& win)
    {
        float w = 30.f;
        sf::RectangleShape bg(Vector2f(w, 4.f));
        bg.setFillColor(Color::Red), bg.setPosition(x - w / 2.f, y + 16.f);
        float ratio = (float)hp / (float)maxhp;
        sf::RectangleShape fg(Vector2f(w * ratio, 4.f));
        fg.setFillColor(Color(0, 200, 50)), fg.setPosition(x - w / 2.f, y + 16.f);
        win.draw(bg);

        win.draw(fg);
    }
public:
    Tower(float x, float y, float r, float dam, float FR, int cst, int hp = 200) : Entity(x, y, hp)
    {
        range = r, damage = dam, fcooldown = 0, cost = cst, fRate = FR;
        level = 1, maxLevel = 3, upgradeCost = cst * 2 / 3;
        dmgMult = 1, fRateMult = 1;
        upgradeDamageFactor = 0;
        upgradeRangeFactor = 0;
    }
    virtual void upgrade()
    {
    if (level >= maxLevel)
        return;
    damage *= upgradeDamageFactor;
    range *= upgradeRangeFactor;
    level++;
    upgradeCost = (int)(upgradeCost * 1.5f); 
    }
    int getLevel() const 
    {
        return level;
    }
    int getUpgradeCost() const
    { 
        return upgradeCost;
    }
    bool canUpgrade() const
    { 
        return level < maxLevel;
    }
    void update(float dt)
    {
        if (fcooldown > 0.f)
            fcooldown -= dt;
    }
    bool canFire() const 
    { 
        return fcooldown <= 0.f;
    }
    Enemy* findTarget(Enemy** e, int n)
    {
        for (int i = 0; i < n; i++)
        {
            if (!e[i]->isAlive())
                continue;
            float dx, dy;
            dx = e[i]->getX() - x;
            dy = e[i]->getY() - y;
            if (std::sqrt(dx * dx + dy * dy) <= range)
                return e[i];
        }
        return nullptr;
    }
    void setBuffs(float Di, float Fi) // Di-> damage increase factor, Fi-> Rate increase FActor
    {
        dmgMult = Di;
        fRateMult = Fi;
    }
    float getEffectiveFireRate()const
    {
        return fRate * fRateMult;  
    }
    int getCost()const
    {
        return cost; 
    }
    float getRange()const
    {
        return range;
    }
    float getEffectiveDamage() const
    {
        return damage * dmgMult;
    }
    virtual Bullet* attack(Enemy** enemies, int count) = 0;
    virtual void render(sf::RenderWindow& window) override = 0;
    virtual ~Tower() {}
};

#endif