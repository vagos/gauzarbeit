#include "player/Player.hpp"

#include "player/PlayerAchiever.hpp"
#include "player/PlayerAttackable.hpp"
#include "player/PlayerInspectable.hpp"
#include "player/PlayerNetworked.hpp"
#include "player/PlayerNotifier.hpp"
#include "player/PlayerPhysical.hpp"
#include "player/PlayerTalker.hpp"
#include "player/PlayerThinker.hpp"

#include <memory>

Player::Player()
{
    _networked = std::make_unique<PlayerNetworked>();
    _physical = std::make_unique<PlayerPhysical>();
    _attackable = std::make_unique<PlayerAttackable>();
    _notifier = std::make_unique<PlayerNotifier>();
    _talker = std::make_unique<PlayerTalker>();
    _achiever = std::make_unique<PlayerAchiever>();
    _inspectable = std::make_unique<PlayerInspectable>();
    _thinker = std::make_unique<PlayerThinker>();
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
    playerCommands["quit"] = Event::Type::Leave;

    playerCommands["do"] = Event::Type::Do;
    playerCommands["greet"] = Event::Type::Greet;

    playerCommands["ask"] = Event::Type::Ask;

    playerCommands["help"] = Event::Type::Help;

    playerCommands["move"] = Event::Type::Move;
    playerCommands["go"] = Event::Type::Move;

    playerCommands["pickup"] = Event::Type::Gain;
    playerCommands["grab"] = Event::Type::Gain;

    playerCommands["look"] = Event::Type::Look;

    playerCommands["tell"] = Event::Type::Say;
    playerCommands["say"] = Event::Type::Say;

    playerCommands["register"] = Event::Type::Register;
}

std::unordered_map<std::string, Event::Type> Player::playerCommands;
