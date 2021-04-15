#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"
#include <memory>
#include <sstream>

void PlayerPhysical::doUpdate( std::shared_ptr<Thing> owner)
{

    std::stringstream requestStream { owner -> networked -> getRequestStream().str() };

    std::string sVerb; 

    requestStream >> sVerb;

    if (sVerb == "move")
    {
        std::string sDirection;

        requestStream >> sDirection;

        moveDirection(owner, sDirection);

    }

    else if (sVerb == "look")
    {
        std::stringstream res;

        res << "\n\n" << "Players in the room: \n";

        for (const auto& thing : currentRoom -> listPlayers) 
        {
            res << "-- " << *thing << "\n";
        }

        owner -> networked -> addResponse( res.str() );

    }

    else if (sVerb == "inventory")
    {
        std::stringstream res;

        res << owner -> sName << "'s Inventory: \n";

        for (const auto& item : tInventory)
        {
            res << *item << "\n";
        }

        owner -> networked -> addResponse( res.str() );

    }


}



void PlayerPhysical::moveDirection(std::shared_ptr<Thing> owner, const std::string& direction)
{

        if (direction == "left")
        {
            doMove(owner, currentRoom -> x - 1, currentRoom -> y);
        }
        
        else if (direction == "right")
        {
            doMove(owner, currentRoom -> x + 1, currentRoom -> y);
        }
   
        else if (direction == "down")
        {
            doMove(owner, currentRoom -> x, currentRoom -> y + 1);
        }
   
        else if (direction == "up")
        {
            doMove(owner, currentRoom -> x, currentRoom -> y - 1);
        }

        else return;

        std::stringstream res; 

        res << "Moved into Room: " << currentRoom -> x << " " << currentRoom -> y << "\n";

        owner -> networked -> addResponse( res.str() );

}