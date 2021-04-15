#include "Player.hpp"

#include "PlayerNetworked.hpp"
#include "PlayerPhysical.hpp"



Player::Player()
{
    networked = std::make_unique<PlayerNetworked>();
    physical = std::make_unique<PlayerPhysical>();
}
