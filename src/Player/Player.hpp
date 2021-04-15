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
    Player(); 
    
    void doInit();

    const std::string getInfo() const override
    {
        return " (Player)";
    }

};

#endif//PLAYER_HPP
