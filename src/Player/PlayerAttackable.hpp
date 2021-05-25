#ifndef PLAYER_ATTACKABLE_HPP
#define PLAYER_ATTACKABLE_HPP

#include "../Thing.hpp"
#include "../Room.hpp"

class PlayerAttackable : public Attackable
{
public:
    PlayerAttackable()
    {

    }
    
    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {
        auto event = owner -> notifier -> event;

        if (event.verb == "attack")
        {
            auto enemy = owner -> physical -> getRoom() -> getThing( event.noun );

            if (!enemy) return;

            if (enemy -> attackable)
            {
                doAttack( owner, enemy );
            }

            else 
            {
                owner -> networked -> addResponse( ColorString ("You can't attack that!\n", Color::Red) );
            }
        }

    }

    virtual void doAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &target) override
    {

        std::stringstream res;

        res << "You attacked " << target -> name << " for " << attack << "\n";

        owner -> networked -> addResponse( res.str() );
        
        Attackable::doAttack(owner, target);
        
    }
    

};

#endif
