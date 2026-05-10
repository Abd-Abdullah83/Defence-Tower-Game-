#pragma once
#ifndef Entity_H
#define Entity_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace sf;

class Entity {
protected:
    float x, y;
    int hp, maxhp;
    bool alive;

public:
    Entity(float a, float b, int h)
    {
        x = a, y = b, hp = h,maxhp=h;
        alive = true;
    }
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void takeDamage(int n)
    {
        hp -= n;
        if (hp <= 0)
        {
            hp = 0;
            alive = false;
        }
    }
    bool isAlive() const
    { 
        return alive; 
    }
    float getX()  const 
    {
        return x;
    }
    float getY()   const 
    { 
        return y;
    }
    int getHp()    const 
    {
        return hp;
    }
    int getMaxHp() const
    { 
        return maxhp; 
    }
    bool operator==(const Entity& other) const //== function(Overloading as per requirement of OOP)
    {
        return (x == other.x && y == other.y);
    }
    virtual ~Entity() 
    {}
};

#endif
