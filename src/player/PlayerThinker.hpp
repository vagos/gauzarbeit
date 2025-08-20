#ifndef PLAYER_THINKER_HPP
#define PLAYER_THINKER_HPP

#include "thing/Thing.hpp"

class PlayerThinker : public Thinker
{
  public:
    PlayerThinker() {}

    void doThink(const std::shared_ptr<Thing>& owner) override;
};

#endif
