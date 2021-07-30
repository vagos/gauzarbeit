#ifndef PLAYER_TALKER_HPP
#define PLAYER_TALKER_HPP

#include <iomanip>
#include <cassert>
#include <memory>
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
                break;
            }
            
            case Event::Type::Help:
            {
                if (event.target.size())
                {
                    auto t = owner -> physical() -> getItem( event.target );

                    if (!t) throw TargetNotFound();

                    owner -> networked() -> addResponse( t -> inspectable() -> getName(t) + ": " + t -> inspectable() -> onHelp(t, owner) );
                }

                else 
                {

                    std::stringstream res; res << '\n' << "A HelpLeaflet materializes in your pocket. Type \"USE HelpLeaflet\" to read it.\n\n";

                    owner -> physical() -> gainItem( std::make_shared<ScriptedThing>("HelpLeaflet") );

                    owner -> networked() -> addResponse(res.str()); 
 
                }

                break;
            }

            case Event::Type::Ask:
            {
                auto t = owner -> physical() -> current_room -> getThing(event.target);

                if (!t) throw TargetNotFound();

                if (event.object.size())
                {
                    std::stringstream res; res << '\n' << "You ask " << *t << " about " << event.object << '\n';
                    owner -> networked() -> addResponse( ColorString( res.str(), Color::Yellow) );
                }
                
                t -> talker() -> onTalk(t, owner); 

                break;
            }

            case Event::Type::Say:
            {
                std::stringstream res;

                if (event.target == "everyone") 
                {
                    res << "You said: " << std::quoted( event.object + event.extra ) << '\n';

                    owner -> networked() -> addResponse(res.str());

                    owner -> notifier() -> doNotify(owner, Event::Type::Chat);

                    break;

                }

                auto p = owner -> physical() -> current_room -> getPlayer( event.target );

                if (!p) throw TargetNotFound();
                
                std::stringstream whisper;

                whisper << '\n' << owner -> name << " whispered to you: " << std::quoted(event.object + " " + event.extra) << '\n';
                
                p -> networked() -> addResponse( ColorString( whisper.str(), Color::Yellow ) );

                break;
            }

        }


        if (event.verb == "guild")
        {
           if (!guild) 
           {
               createGuild(owner, event.target); return;

               owner -> networked() -> addResponse( "You created a Guild!\n" + guild -> onInspect() ); 
           }
           
           if (!event.target.size()) owner -> networked() -> addResponse( guild -> onInspect() );
            
           else 
           {
              auto p = owner -> physical() -> current_room -> getPlayer( event.target );

              if (!p) throw TargetNotFound();

              owner -> networked() -> addResponse("You added " + p -> name + " to you guild!\n");

              p -> networked() -> addResponse("You were added to guild " + guild -> name + '\n');

              guild -> addMember(owner, p);
           }
        
        }
    }
        
/* 

        if ( event.verb == "do" )
        {
            auto p = owner -> physical -> current_room -> getPlayer( event.target );

            if (p) p -> notifier -> onNotify(p, owner, Event::Type::Info);
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

    }

    
     void onTalk(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> talker) override
     {
         Talker::onTalk(owner, talker);
     } 
    */
};

#endif
