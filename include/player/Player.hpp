#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "thing/Thing.hpp"

#include <memory>
#include <sstream>
#include <unordered_map>

class Player : public Thing
{
  public:
    Player();

    static std::unordered_map<std::string, Event::Type> playerCommands;

    static void setPlayerCommands();
};

#endif // PLAYER_HPP
