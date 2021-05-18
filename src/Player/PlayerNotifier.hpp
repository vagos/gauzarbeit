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

        for (auto& p : owner -> physical -> getRoom() -> listPlayers)
        {
            p -> notifier -> onNotify(p, owner, notification_type);
        }

        for (auto& quest : owner -> talker -> getQuests())
        {
           quest -> getNotified(owner, notification_type, nullptr); 
        }
    }

    void onNotify(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> actor, Event::Type notification_type) override
    {
        switch (notification_type)
        {
            
            case Event::Type::Whisper:
                {
                    if ( owner -> notifier -> event.noun == owner -> sName)
                    {
                            owner -> networked -> addResponse(actor -> sName + 
                            "whispered to you: " + actor -> notifier -> event.extra + "\n");
                    }

                    break;
                }
        }
    }

    void setEvent(std::shared_ptr<Thing> owner) override
    {
        std::stringstream req{ owner -> networked -> getRequestStream().str() };

        req >> event.verb >> event.noun;

        std::getline( req, event.extra );

        std::clog << event.verb << " " << event.noun << " " << event.extra << "\n";
        
    }
   
};

#endif
