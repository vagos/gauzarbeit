#ifndef PLAYER_NOTIFIER_HPP
#define PLAYER_NOTIFIER_HPP


#include "../Thing.hpp"
#include "../Quest.hpp"
#include "PlayerPhysical.hpp"


class PlayerNotifier : public Notifier
{

    void doNotify(std::shared_ptr<Thing> owner, Event::Type notification_type, std::shared_ptr<Thing> target = nullptr) override
    {
        for (auto& thing : owner -> physical -> getRoom() -> listThings)
        {
            if ( thing -> notifier )
            {
                thing -> notifier -> onNotify(thing, owner, notification_type); 
            }

        }

        for (auto& quest : owner -> talker -> getQuests())
        {
           quest -> getNotified(owner, notification_type, nullptr); 
        }
    }
   
};

#endif
