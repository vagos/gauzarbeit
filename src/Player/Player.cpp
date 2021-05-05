#include "Player.hpp"

#include "PlayerNetworked.hpp"
#include "PlayerPhysical.hpp"
#include "PlayerAttackable.hpp"
#include <memory>

Player::Player()
{
    networked  = std::make_unique<PlayerNetworked>();
    physical   = std::make_unique<PlayerPhysical>();
    attackable = std::make_unique<PlayerAttackable>();
}
