#include "Player.hpp"

#include "PlayerPhysical.hpp"
#include "PlayerAttackable.hpp"
#include "PlayerNotifier.hpp"
#include "PlayerTalker.hpp"
#include "PlayerInspectable.hpp"
#include "PlayerNetworked.hpp"

#include <memory>

Player::Player()
{
    _networked  = std::make_unique<PlayerNetworked>();
    _physical   = std::make_unique<PlayerPhysical>();
    _attackable = std::make_unique<PlayerAttackable>();
    _notifier = std::make_unique<PlayerNotifier>();
    _talker = std::make_unique<PlayerTalker>();
    _achiever = std::make_unique<Achiever>();
    _inspectable = std::make_unique<PlayerInspectable>();

}

void Player::setPlayerCommands()
{
    playerCommands["inv"] = Event::Type::Inspect;
    playerCommands["inspect"] = Event::Type::Inspect;
    playerCommands["check"] = Event::Type::Inspect;

    playerCommands["attack"] = Event::Type::Attack;
    playerCommands["use"] = Event::Type::Use;
    playerCommands["login"] = Event::Type::Enter;
    playerCommands["exit"] = Event::Type::Leave;
    playerCommands["do"] = Event::Type::Do;
    playerCommands["greet"] = Event::Type::Greet;
    playerCommands["ask"] = Event::Type::Ask;

    playerCommands["move"] = Event::Type::Move;
    playerCommands["go"] = Event::Type::Move;

    playerCommands["pickup"] = Event::Type::Gain;
    playerCommands["grab"] = Event::Type::Gain;
}


std::unordered_map<std::string, Event::Type> Player::playerCommands;
