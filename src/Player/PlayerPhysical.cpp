#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"
#include "../World.hpp"
#include <memory>
#include <sstream>

#include "../Helpers.hpp"
#include "../Script//ScriptedThing.hpp"

void PlayerPhysical::doUpdate( const std::shared_ptr<Thing> &owner, World & world)
{

    auto event =  owner -> notifier -> event;

    if ( event.verb == "move"  )
    {
        moveDirection(owner, event.extra);
    }

    else if( event.verb == "get" )
    {
        gainItem( std::make_shared<ScriptedThing>(event.noun) );
    }

    else if ( event.verb == "catch" )
    {
        owner -> notifier -> doNotify(owner, Notifier::Event::Type::Catch);
    }

    else if ( event.verb == "look" )
    {
        std::stringstream res;

        res << "Players in the room: \n";

        for (const auto& thing : current_room -> listPlayers) 
        {
            res << "-- " << *thing << "\n";
        }
        
        res << "\n\n" << "Other things in the room: \n";
        
        for (const auto& thing : current_room -> listThings) 
        {
            res << "-- " << *thing << "\n";
        }
       
        owner -> networked -> addResponse( res.str() );

    }

    else if ( event.verb.substr(0, 3) == "inv" )
    {
        std::stringstream res;

        res << owner -> name << "'s Inventory: \n";

        for (const auto& item : inventory)
        {
            res << "-- " << *item << "\n";
        }

        owner -> networked -> addResponse( res.str() );

    }

    else if (event.verb == "use")
    {
        std::shared_ptr<Thing> t;

        if (IsNumber(event.noun))
            t = getItem(std::atoi(event.noun.c_str()));
        else
            t = getItem(event.noun);
    
        if (t)
            t -> usable -> onUse(t, owner);
    }

    else if (event.verb == "give") // fix this
    {
        auto p = world.getPlayer(event.noun);

        if (!p) 
        {
            owner -> networked -> addResponse( ColorString("Player not found!\n", Color::Red) );
            return;
        }

        auto item = getItem( event.extra );

        if (!item) return;
         
        p -> physical -> gainItem( item );

        std::stringstream message;
        message << "You were given a " << item -> name << " by " << owner -> name << "\n"; 
        p -> networked -> addResponse( message .str() );
        owner -> networked -> addResponse("You gave " + event.noun + " your " + item -> name + "\n");
        
        loseItem(item);
    }

}



void PlayerPhysical::moveDirection(std::shared_ptr<Thing> owner, const std::string& direction)
{

        if (direction == "left")
        {
            doMove(owner, current_room -> x - 1, current_room -> y);
        }
        
        else if (direction == "right")
        {
            doMove(owner, current_room -> x + 1, current_room -> y);
        }
   
        else if (direction == "down")
        {
            doMove(owner, current_room -> x, current_room -> y + 1);
        }
   
        else if (direction == "up")
        {
            doMove(owner, current_room -> x, current_room -> y - 1);
        }

        else return;

        std::stringstream res; 

        res << "Moved into Room: " << current_room -> x << " " << current_room -> y << "\n";

        owner -> networked -> addResponse( res.str() );

}

void PlayerPhysical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room) current_room -> removePlayer(owner);

    current_room = Room::get(x, y); 

    current_room -> addPlayer(owner);

}
