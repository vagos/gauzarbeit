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
    }
/*        auto& event = owner -> notifier() -> event;

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
