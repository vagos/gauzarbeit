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

        if (!alive)
        {
            doRespawn(owner);
        }

        if (event.verb == "attack")
        {
            auto enemy = owner -> physical -> getRoom() -> getThing( event.target );

            if (!enemy) 
                owner -> networked -> addResponse("There doesn't seem to be anything with that name here!\n");

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

    void onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker) override
    {
        std::stringstream res;

        res << attacker -> name << " attacked you!\n";

        owner -> networked -> addResponse(res.str());

        Attackable::onAttack(owner, attacker);
    }

    void onDeath(const std::shared_ptr<Thing> &owner) override
    {
        alive = false;

        owner -> networked -> addResponse( ColorString("You died!\n", Color::Red) );

        owner -> notifier -> doNotify(owner, Notifier::Event::Type::Death); // notify everyone about the death
    }

    void doRespawn(const std::shared_ptr<Thing> owner)
    {
        alive = true;

        owner -> physical -> doMove(owner, 0, 0);

        owner -> networked -> addResponse("You respawned!\n");
    }
    

};

#endif
