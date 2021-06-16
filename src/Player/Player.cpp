#include "Player.hpp"

#include <memory>

Player::Player()
{
    networked  = std::make_unique<PlayerNetworked>();
    physical   = std::make_unique<PlayerPhysical>();
    attackable = std::make_unique<PlayerAttackable>();
    notifier = std::make_unique<PlayerNotifier>();
    talker = std::make_unique<PlayerTalker>();
    achiever = std::make_unique<Achiever>();
    inspectable = std::make_unique<PlayerInspectable>();

}
