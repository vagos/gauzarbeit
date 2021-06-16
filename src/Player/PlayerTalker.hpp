#ifndef PLAYER_TALKER_HPP
#define PLAYER_TALKER_HPP

#include <iomanip>
#include <cassert>


#include "../Thing.hpp"
#include "../Quest.hpp"
#include "../Room.hpp"

class PlayerTalker : public Talker
{
    
    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {
        auto& event = owner -> notifier -> event;
        
        std::stringstream res;

        if ( event.verb == "quests")
        {
            res << owner -> name << "'s Quests: \n";

            res << VerticalListString(owner->achiever->quests, '*' ,
            [](const Thing& t) {return t.name;});

            owner -> networked -> addResponse(res.str());
        }
 
        if ( event.verb == "say" && event.object == "aloud")
        {
            res << "You said: " << std::quoted( event.extra ) << '\n';

            owner -> networked -> addResponse(res.str());

            owner -> notifier -> doNotify(owner, Notifier::Event::Type::Chat);
        }

        if ( event.verb == "tell" )
        {
            auto p = owner -> physical -> getRoom() -> getPlayer( event.target );

            if (!p)
            {
                owner -> networked -> addResponse( ColorString("Player not found!\n", Color::Red) );
                return;
            }
            
            std::stringstream whisper;

            whisper << owner -> name << " whispered to you: " << std::quoted(owner -> notifier -> event.extra) << '\n';
            
            p -> networked -> addResponse( ColorString( whisper.str(), Color::Yellow ) );
 
        }

        if ( event.verb == "inform" )
        {
            auto p = owner -> physical -> current_room -> getPlayer( event.target );

            if (p) p -> notifier -> onNotify(p, owner, Notifier::Event::Type::Info);
            else owner -> networked -> addResponse( ColorString("No player with that name found!\n", Color::Red) );
        }
        
        if ( event.verb == "buy" )
        {
            auto t = owner -> physical -> current_room -> getThing( event.target );

            if (t)
            {
                assert(t -> notifier);
                t -> notifier -> onNotify(t, owner, Notifier::Event::Type::Buy);
            }
        }

        if (event.verb == "ask")
        {
            auto t = owner -> physical -> current_room -> getThing(event.target);

            if (t) t -> notifier -> onNotify(t, owner, Notifier::Event::Type::Ask);
            else owner -> networked -> addResponse( ColorString("Nothing with that name found!\n", Color::Red) );
        }
    }
};

#endif
