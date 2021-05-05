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
            auto enemy = * (owner -> physical -> getRoom() -> listThings.begin() ) ;

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
    

};

#endif
