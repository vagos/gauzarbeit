#ifndef PLAYER_TALKER_HPP
#define PLAYER_TALKER_HPP

#include <iomanip>

#include "../Thing.hpp"
#include "../Quest.hpp"

class PlayerTalker : public Talker
{
    
    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {
        auto& event = owner -> notifier -> event;
        
        std::stringstream res;

        if ( event.verb == "quests")
        {
            res << owner -> name << "'s Quests: \n";

            for (auto& quest : owner -> achiever -> quests)
            {
                res << quest -> name << "\n";
            }

            owner -> networked -> addResponse(res.str());
        }

        if ( event.verb == "say" )
        {
            res << "You said: " << std::quoted(event.noun + event.extra) << '\n';

            owner -> networked -> addResponse(res.str());

            owner -> notifier -> doNotify(owner, Notifier::Event::Type::Say);

        }

        
    }

};

#endif
