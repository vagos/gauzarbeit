#ifndef PLAYER_THINKER_HPP
#define PLAYER_THINKER_HPP

#include "../Thing.hpp"

class PlayerThinker : public Thinker
{
    void doThink(const std::shared_ptr<Thing> &owner) override;

};

#endif
