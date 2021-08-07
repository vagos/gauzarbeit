#ifndef ATTACKABLE_HPP
#define ATTACKABLE_HPP

#include <memory>

class Thing;

class Attackable
{
public:

    Attackable(): alive(true)
    {

    }

    virtual void doAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &target);
    virtual void onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker);
    
    virtual void getDamaged(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker, int dmg);
    
    virtual void doUpdate(const std::shared_ptr<Thing> &owner)
    {

    }

    virtual void onDeath(const std::shared_ptr<Thing>& owner);
    
    void setMaxHealth(int max_hp)
    {
        max_health = max_hp;
        current_health = max_health;
    }

    bool is_alive() {return alive;}


public:

    int max_health, attack = 1, defense = 0;
    float current_health;

protected:
    bool alive;

};



#endif//ATTACKABLE_HPP
