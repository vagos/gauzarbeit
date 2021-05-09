#ifndef PLAYER_TALKER_HPP
#define PLAYER_TALKER_HPP

#include "../Thing.hpp"
#include "../Quest.hpp"

class PlayerTalker : public Talker
{
    
    void doUpdate(std::shared_ptr<Thing> owner) override
    {
        std::stringstream req { owner -> networked -> getRequestStream().str() };
        
        std::stringstream res;
        
        std::string verb;

        req >> verb;

        if ( verb == "quests")
        {
            res << owner -> sName << "'s Quests: \n";

            for (auto& quest : quests)
            {
                res << quest -> name << "\n";
            }

            owner -> networked -> addResponse(res.str());
        }

        if ( verb == "say" )
        {
            std::string phrase; 
            
            std::getline(req, phrase);

            phrase = phrase.substr(1, phrase.size());

            res << "You said: \"" << phrase << "\" \n";

            owner -> networked -> addResponse(res.str());

            owner -> notifier -> doNotify(owner, Notifier::Event::Type::Say);

        }

        
    }

};

#endif
