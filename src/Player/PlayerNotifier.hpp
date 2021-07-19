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
        for (auto& thing : owner -> physical() -> current_room -> things)
        {
            if ( thing -> _notifier )
            {
                thing -> notifier() -> onNotify(thing, owner, notification_type, target); 
            }

        }

        for (auto& p : owner -> physical() -> getRoom() -> players)
        {
            if (p == owner) continue;

            p -> notifier() -> onNotify(p, owner, notification_type, target);
        }


        for (auto& t : owner -> physical() -> inventory)
        {
            if (t -> _notifier) t -> notifier() -> onNotify(t, owner, notification_type, target);
        }

        for (auto& quest : owner -> achiever() -> quests)
        {
           quest -> notifier() -> onNotify(quest, owner, notification_type, target); 
        }
    }

    void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type, const std::shared_ptr<Thing>& target) override
    {
        switch (notification_type)
        {

           case Event::Type::Info: // The player passes the event info into their inventory items.
           {
                for (const auto& t : owner -> physical() -> inventory)
                {
                    if (t -> _notifier) t -> notifier() -> onNotify(t, actor, notification_type, target);
                }
                break;
           }
            
           case Event::Type::Chat:
           {
                std::stringstream chat;

                chat << actor->name << ": " << actor -> notifier() -> event.object << actor -> notifier() -> event.extra << '\n';
                owner -> networked() -> addResponse( chat.str() );
                break;
            }

            case Event::Type::Message:
            {
                owner -> networked() -> addResponse( actor -> notifier() -> event.payload );
                break;
            }

            case Event::Type::Kill:
            {
                if ( actor != owner ) break;

                std::stringstream msg;
                msg << "You killed " << target -> name << '\n';
                owner -> networked() -> addResponse(ColorString( msg.str(), Color::Red) );

                doNotify(owner, Event::Type::Kill, target);
                owner -> achiever() -> gainXP(target -> achiever() -> getLevel());

                break;
            }
        }
    }

    void setEvent(const std::shared_ptr<Thing> &owner) override;
       
};

#endif
