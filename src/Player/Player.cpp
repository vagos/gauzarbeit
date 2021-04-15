#include "Player.hpp"

#include "PlayerNetworked.hpp"
#include "PlayerPhysical.hpp"



void Player::doInit()
{
    networked = std::make_unique<PlayerNetworked>(shared_from_this());
    physical = std::make_unique<PlayerPhysical>(shared_from_this());
}
