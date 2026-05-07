#ifndef PLAYER_ACHIEVER_HPP
#define PLAYER_ACHIEVER_HPP

#include "thing/Achiever.hpp"
#include <iomanip>

class PlayerAchiever : public Achiever
{
  public:
    PlayerAchiever();

    void getRewards(const std::shared_ptr<Thing> owner, int size) override;
};

#endif
