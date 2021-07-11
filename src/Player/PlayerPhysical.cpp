#include "../Helpers.hpp"
#include "../World.hpp"
#include <memory>
#include <sstream>

#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"
#include "../Script//ScriptedThing.hpp"

void PlayerPhysical::doUpdate( const std::shared_ptr<Thing> &owner, World & world)
{

    auto event =  owner -> notifier() -> event;

    switch ( event.type )
    {
        case Event::Type::Inspect:
        {
            
            std::shared_ptr<Thing> t;
                
            if (event.target.size() == 0)
            {
                owner -> networked() -> addResponse(owner -> inspectable() -> onInspect(owner, owner));
                break;
            }

            t = current_room -> getAnything( event.target );

            if (t) 
            {
                owner -> networked() -> addResponse(t -> inspectable() -> onInspect(t, owner));
                goto Notify;
            }

            t = owner -> achiever() -> getQuest( event.target );

            if (t)
            {
                owner -> networked() -> addResponse(t -> inspectable() -> onInspect(t, owner));
                goto Notify;
            }

            t = owner -> physical() -> getItem( event.target );

            if (t)
            {
                owner -> networked() -> addResponse(t -> inspectable() -> onInspect(t, owner));
                goto Notify;
            }

            throw TargetNotFound();

            Notify:

            owner -> notifier() -> doNotify(owner, event.type, t); break;
        }

        case Event::Type::Use:
        {
             std::shared_ptr<Thing> t;

            if (IsNumber(event.target))
                t = getItem(std::atoi(event.target.c_str()));
            else
                t = getItem(event.target);

            if (!t) 
            {
                t = current_room -> getThing(event.target);
            }

            if (!t) throw TargetNotFound();
        
            t -> usable() -> onUse(t, owner);

            break;
        }

        case Event::Type::Move:
        {
            moveDirection(owner, event.target);
            owner -> notifier() -> doNotify(owner, event.type);
            break;
        }
    }

    if( event.verb == "get" )
    {
        gainItem( std::make_shared<ScriptedThing>(event.target) );
    }

    if ( event.verb == "spawn" )
    {
        auto t = std::make_shared<ScriptedThing>(event.target);

        t -> physical() -> doMove(t, current_room -> x, current_room -> y);
    }

    if ( event.verb == "generate" )
    {
        int x, y;
        std::string r_t;

        std::stringstream ss{owner -> networked() -> getRequestStream().str()};

        ss >> r_t >> r_t >> x >> y;

        auto t = Room::get(event.target, x, y);
    
        doMove(owner, x ,y);

        std::stringstream res;

        res << "You are in Room " << x << ' ' << y << '\n';

        owner -> networked() -> addResponse(res.str());
    }

    if ( event.verb == "look" )
    {
        owner -> networked() -> addResponse( current_room -> onInspect(current_room, owner) );
    }

//    else if (event.verb == "give") // fix this
//    {
//        auto p = world.getPlayer(event.target);
//
//        if (!p) 
//        {
//            owner -> networked -> addResponse( ColorString("Player not found!\n", Color::Red) );
//            return;
//        }
//
//        auto item = getItem( event.extra );
//
//        if (!item) return;
//         
//        p -> physical -> gainItem( item );
//
//        std::stringstream message;
//        message << "You were given a " << item -> name << " by " << owner -> name << "\n"; 
//        p -> networked -> addResponse( message .str() );
//
//        owner -> networked -> addResponse("You gave " + event.target + " your " + item -> name + "\n");
//        
//        loseItem(item);
//    }

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

        owner -> networked() -> addResponse( res.str() );

}

void PlayerPhysical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room) current_room -> removePlayer(owner);

    current_room = Room::get(x, y); 

    current_room -> addPlayer(owner);
}
