#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"
#include "../World.hpp"
#include <memory>
#include <sstream>

void PlayerPhysical::doUpdate( std::shared_ptr<Thing> owner, World & world)
{

    std::stringstream req { owner -> networked -> getRequestStream().str() }; // The request

    std::string sVerb; 

    req >> sVerb;

    auto event =  owner -> notifier -> event;

    if ( sVerb == "move"  )
    {
        std::string direction;

        req >> direction;

        moveDirection(owner, direction);

    }

    else if (sVerb == "look")
    {
        std::stringstream res;

        res << "Players in the room: \n";

        for (const auto& thing : currentRoom -> listPlayers) 
        {
            res << "-- " << *thing << "\n";
        }
        
        res << "\n\n" << "Other things in the room: \n";
        
        for (const auto& thing : currentRoom -> listThings) 
        {
            res << "-- " << *thing << "\n";
        }
       
        owner -> networked -> addResponse( res.str() );

    }

    else if (sVerb == "inventory" || sVerb == "items" || sVerb == "inv")
    {
        std::stringstream res;

        res << owner -> sName << "'s Inventory: \n";

        for (const auto& item : tInventory)
        {
            res << "-- " << *item << "\n";
        }

        owner -> networked -> addResponse( res.str() );

    }

    else if (sVerb == "use")
    {
        std::shared_ptr<Thing> t;

        if (IsNumber(event.noun))
            t = getItem(std::atoi(event.noun.c_str()));
        else
            t = getItem(event.noun);
    
        if (t)
            t -> usable -> onUse(t, owner);
    }

    else if (sVerb == "give")
    {
        std::string recipient;
        int itemIndex;

        req >> recipient >> itemIndex;

        auto pRecipient = world.getPlayer(recipient);

        if (!pRecipient) return;
         
        pRecipient -> physical -> gainItem( tInventory [itemIndex] );

        std::stringstream message;

        message << "You were given a " << tInventory[itemIndex] -> sName << " by " << owner -> sName << "\n"; 

        owner -> networked -> addMessage( message.str() , recipient);

        auto item = tInventory[itemIndex];

        owner -> networked -> addResponse("You gave " + recipient + " your " + item -> sName + "\n");
        
        tInventory.erase( tInventory.begin() + itemIndex );
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

void PlayerPhysical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (currentRoom) currentRoom -> removePlayer(owner);

    currentRoom = Room::get(x, y); 

    currentRoom -> addPlayer(owner);

}
