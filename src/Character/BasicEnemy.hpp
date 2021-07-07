#include "../Thing.hpp"
#include <memory>
#include <unordered_set>

#include <cassert>

class BasicEnemyAttackable : public Attackable
{

public:

    void onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker) override
    {
        enemies.insert(attacker);

        std::clog << "added " << attacker -> name << " as an enemy\n";

        Attackable::onAttack(owner, attacker);
    }

    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {
       for (auto& e: enemies)
       {
            doAttack(owner, e);
       }

       //std::erase_if(enemies, [](const auto& e){return !e -> attackable() -> is_alive();});
        
    }

    std::unordered_set< std::shared_ptr<Thing> > enemies;
};


class BasicEnemyNotifier : public Notifier
{
public:

    void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type, const std::shared_ptr<Thing> &target) override
    {
        switch ( notification_type )
        {
            case Event::Type::Death:
            {
               std::clog << "My enemy died " << actor -> name << '\n';
            }
        }
    }
    

};

class BasicEnemy : public Thing
{
public:
    BasicEnemy(const std::string& name): Thing(name)
    {
        //_attackable = std::make_unique<BasicEnemyAttackable>();
        _notifier = std::make_unique<BasicEnemyNotifier>();
        _physical = std::make_unique<Physical>();

        //attackable() -> setMaxHealth(2);
    }

};
