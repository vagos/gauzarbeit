#ifndef PLAYER_ACHIEVER_HPP
#define PLAYER_ACHIEVER_HPP

#include "../Thing.hpp"

#include <iomanip>

class PlayerAchiever : public Achiever    
{
public:

    PlayerAchiever();

    void onQuestComplete(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &quest) override;
    void getRewards(const std::shared_ptr<Thing> owner, int size) override;
};


#endif
