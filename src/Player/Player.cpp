#include "Player.hpp"

#include "PlayerNetworked.hpp"
#include "PlayerPhysical.hpp"
#include "PlayerAttackable.hpp"
#include "PlayerNotifier.hpp"
#include "PlayerTalker.hpp"

Player::Player()
{
    networked  = std::make_unique<PlayerNetworked>();
    physical   = std::make_unique<PlayerPhysical>();
    attackable = std::make_unique<PlayerAttackable>();
    notifier = std::make_unique<PlayerNotifier>();
    talker = std::make_unique<PlayerTalker>();
}
