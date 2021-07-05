#ifndef PLAYER_TALKER_HPP
#define PLAYER_TALKER_HPP

#include <iomanip>
#include <cassert>
#include <sstream>


#include "../Thing.hpp"
#include "../Quest.hpp"
#include "../Room.hpp"

class PlayerTalker : public Talker
{
    
    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {
        auto& event = owner -> notifier() -> event;

        switch ( event.type )
        {
            case Event::Type::Do:
            {
                owner -> notifier() -> doNotify(owner, Event::Type::Do, nullptr);
                break;
            }

            case Event::Type::Greet:
            {
                auto t = owner -> physical() -> current_room -> getThing( event.target );

                t -> notifier() -> onNotify(t, owner, Event::Type::Greet);
            }

            case Event::Type::Ask:
            {
                auto t = owner -> physical() -> current_room -> getThing(event.target);

                if (!t) throw TargetNotFound();

                std::stringstream res;
                res << "You ask " << *t << " about " << event.object << '\n';
                owner -> networked() -> addResponse(res.str());
                
                t -> talker() -> onTalk(t, owner); 
            }

        }
        
 
        if ( event.verb == "tell" )
        {
            std::stringstream res;

            if (event.target == "everyone") 
            {
                res << "You said: " << std::quoted( event.object + event.extra ) << '\n';

                owner -> networked() -> addResponse(res.str());

                owner -> notifier() -> doNotify(owner, Event::Type::Chat);

                return;
            }

            auto p = owner -> physical() -> current_room -> getPlayer( event.target );

            if (!p) throw TargetNotFound();
            
            std::stringstream whisper;

            whisper << owner -> name << " whispered to you: " << std::quoted(event.object + event.extra) << '\n';
            
            p -> networked() -> addResponse( ColorString( whisper.str(), Color::Yellow ) );
 
        }

//        if ( event.verb == "do" )
//        {
//            auto p = owner -> physical -> current_room -> getPlayer( event.target );
//
//            if (p) p -> notifier -> onNotify(p, owner, Event::Type::Info);
//            else owner -> networked -> addResponse( ColorString("No player with that name found!\n", Color::Red) );
//        }
        
//        if ( event.verb == "buy" )
//        {
//            auto t = owner -> physical -> current_room -> getThing( event.target );
//
//            if (t)
//            {
//                assert(t -> notifier);
//                t -> notifier -> onNotify(t, owner, Notifier::Event::Type::Buy);
//            }
//        }
//
    }
};

#endif
