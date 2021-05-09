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
    
    void doUpdate(std::shared_ptr<Thing> owner) override
    {
        std::stringstream req { owner -> networked -> getRequestStream().str() };

        std::string verb;

        req >> verb;

        if (verb == "attack")
        {
            auto enemy = * ( owner -> physical -> getRoom() -> listThings.begin() ) ;

            if (enemy -> attackable)
            {
                doAttack( owner, enemy );
            }

            else 
            {
                owner -> networked -> addResponse("You can't attack that!\n");
            }
        }

    }

    void doAttack(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> target) override
    {
        target -> attackable -> current_health -= attack;

        std::stringstream res;

        res << "You attacked " << target -> sName << " for " << attack << "\n";

        res << "He now has " << target -> attackable -> current_health << " HP\n"; 

        owner -> networked -> addResponse( res.str() );
        
    }
    

};

#endif
