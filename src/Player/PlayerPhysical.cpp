#include "../Helpers.hpp"
#include "../World.hpp"
#include <memory>
#include <sstream>

#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"
#include "../Script//ScriptedThing.hpp"

void PlayerPhysical::doUpdate( const std::shared_ptr<Thing> &owner, World & world)
{

    auto event =  owner -> notifier -> event;

    if ( event.verb == "move"  )
    {
        moveDirection(owner, event.target);
    }

    else if( event.verb == "get" )
    {
        gainItem( std::make_shared<ScriptedThing>(event.target) );
    }

    else if ( event.verb == "catch" )
    {
        owner -> notifier -> doNotify(owner, Notifier::Event::Type::Catch);
    }

    else if ( event.verb == "look" )
    {
        std::stringstream res;

        res << HeaderString( VerticalListString(current_room -> players, '*'), "Players in the Room", '=');
        
        res << "\n";

        res << HeaderString( VerticalListString(current_room -> things, '*'), "Other things in the Room", '=');
        
        owner -> networked -> addResponse( res.str() );

    }

    else if ( event.verb == "inspect" )
    {
        if (!event.target.size()) return;

        auto p = current_room -> getPlayer( event.target );

        if (p) owner -> networked -> addResponse(p -> inspectable -> onInspect(p, owner));

        auto t = current_room -> getThing( event.target );

        if (t && t -> inspectable) owner -> networked -> addResponse( t -> inspectable -> onInspect(t, owner) );
    }

    else if ( event.verb.substr(0, 3) == "inv" )
    {
        std::stringstream res;

        res << owner -> name << "'s Inventory: \n\n";

        res << VerticalListString(inventory, '*') << '\n';

        owner -> networked -> addResponse( res.str() );
    }

    else if ( event.verb.substr(0, 3) == "equ" )
    {

        std::stringstream res;

        res << owner -> name << "'s Equipped Items: \n\n";

        res << VerticalListString(equipment, '*') << '\n';

        owner -> networked -> addResponse( res.str() );
    }

    else if (event.verb == "use")
    {
        std::shared_ptr<Thing> t;

        if (IsNumber(event.target))
            t = getItem(std::atoi(event.target.c_str()));
        else
            t = getItem(event.target);
    
        if (t)
            t -> usable -> onUse(t, owner);
    }

    else if (event.verb == "give") // fix this
    {
        auto p = world.getPlayer(event.target);

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

        owner -> networked -> addResponse("You gave " + event.target + " your " + item -> name + "\n");
        
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
