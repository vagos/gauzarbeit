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
        for (auto& thing : owner -> physical -> getRoom() -> listThings)
        {
            if ( thing -> notifier )
            {
                thing -> notifier -> onNotify(thing, owner, notification_type); 
            }

        }

        for (auto& p : owner -> physical -> getRoom() -> listPlayers)
        {
            if (p == owner) continue;

            p -> notifier -> onNotify(p, owner, notification_type);
        }

        for (auto& quest : owner -> achiever -> quests)
        {
           quest -> notifier -> onNotify(quest, owner, notification_type); 

           if (quest -> tasker -> isCompleted())
           {
               quest -> tasker -> giveRewards( quest, owner );
           }
        }
    }

    void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type) override
    {
        switch (notification_type)
        {
            
           case Event::Type::Chat:
           {
                std::stringstream chat;

                chat << actor->name << ": " << actor -> notifier -> event.noun << actor -> notifier -> event.extra << '\n';
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
                msg << "You killed " << actor -> name << '\n';
                
                owner -> networked -> addResponse(ColorString( msg.str(), Color::Red) );

                break;
            }
        }
    }

    void setEvent(const std::shared_ptr<Thing> &owner) override
    {
        std::stringstream req{ owner -> networked -> getRequestStream().str() };

        req >> event.verb >> event.noun;

        std::getline( req >> std::ws, event.extra );
    }
   
};

#endif
