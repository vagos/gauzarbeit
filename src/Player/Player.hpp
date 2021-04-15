#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Thing.hpp"
#include <memory>
#include <sstream>

#include "PlayerNetworked.hpp"
#include "PlayerPhysical.hpp"

class Player : public Thing
{
public:
    Player() 
    {

    }


    void doInit();

    friend std::ostream& operator<<(std::ostream& os, const Player& thing)
    {
        os << thing.sName << " (Player)";

        return os;
    }


};

#endif//PLAYER_HPP
