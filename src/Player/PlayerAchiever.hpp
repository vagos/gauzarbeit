#ifndef PLAYER_ACHIEVER_HPP
#define PLAYER_ACHIEVER_HPP

#include "../Thing.hpp"

#include <iomanip>

class PlayerAchiever : public Achiever    
{

    void onQuestComplete(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &quest) override
    {
        Achiever::onQuestComplete(owner, quest);

        std::stringstream msg; msg << "You completed the Quest: " << std::quoted(quest -> name) << "\n\n";
        
        owner -> networked() -> addResponse( ColorString( msg.str(), Color::Green ) );

    }
    void getRewards(const std::shared_ptr<Thing> owner, int size) override
    {
        Achiever::getRewards(owner, size);
        
        std::stringstream msg; msg << "You gained " << size << " XP!\n\n";

        owner -> networked() -> addResponse( msg.str() );
    }
};


#endif
