#ifndef PLAYER_NOTIFIER_HPP
#define PLAYER_NOTIFIER_HPP


#include "../Thing.hpp"
#include "../Quest.hpp"
#include "PlayerPhysical.hpp"

#include <cassert>
#include <sstream>


class PlayerNotifier : public Notifier
{

    void doNotify(const std::shared_ptr<Thing> &owner, Event::Type notification_type, const std::shared_ptr<Thing> &target = nullptr) override
    {
        for (auto& thing : owner -> physical -> current_room -> things)
        {
            if ( thing -> notifier )
            {
                thing -> notifier -> onNotify(thing, owner, notification_type, target); 
            }

        }

        for (auto& p : owner -> physical -> getRoom() -> players)
        {
            if (p == owner) continue;

            p -> notifier -> onNotify(p, owner, notification_type, target);
        }

        for (auto& quest : owner -> achiever -> quests)
        {
           quest -> notifier -> onNotify(quest, owner, notification_type, target); 

           if (quest -> tasker -> isCompleted())
           {
               quest -> tasker -> giveRewards( quest, owner );
           }
        }
    }

    void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type, const std::shared_ptr<Thing>& target) override
    {
        switch (notification_type)
        {

           case Event::Type::Info: // The player passes the event info into their inventory items.
           {
                for (const auto& t : owner -> physical -> inventory)
                {
                    if (t -> notifier) t -> notifier -> onNotify(t, actor, notification_type, target);
                }
                break;
           }
            
           case Event::Type::Chat:
           {
                std::stringstream chat;

                chat << actor->name << ": " << actor -> notifier -> event.extra << '\n';
                owner -> networked -> addResponse( chat.str() );
                break;
            }

            case Event::Type::Message:
            {
                owner -> networked -> addResponse( actor -> notifier -> event.payload );
                break;
            }

            case Event::Type::Kill:
            {
                std::stringstream msg;
                msg << "You killed " << target -> name << '\n';
                
                owner -> networked -> addResponse(ColorString( msg.str(), Color::Red) );

                doNotify(owner, Event::Type::Kill, target);

                break;
            }
        }
    }

    void setEvent(const std::shared_ptr<Thing> &owner) override
    {
        std::stringstream req{ owner -> networked -> getRequestStream().str() };

        auto words = TokenizeString(req.str());

        if (words.size() >= 4 && req.str().find("from") != std::string::npos) 
        {
            event.verb = words[0];
            event.object = words[1];
            event.target = words[3];
        
            return;
        }

        else if (words.size() >= 4 && req.str().find("with") != std::string::npos)
        {
            event.verb = words[0];
            event.target = words[1];
            event.object = words[3];
            
            return;

        }
        
        else if (words.size() >= 4 && req.str().find("on") != std::string::npos)
        {
            event.verb = words[0];
            event.target = words[1];
            event.object = words[3];
            
            return;
        }
    
        req >> event.verb >> event.target >> event.object;
        
        std::getline( req >> std::ws, event.extra );
    }
   
};

#endif
