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
    
    const std::string getInfo() const override
    {
        std::stringstream ss;
        
        ss << "(Player) " << "[" << networked -> getID() << "]"; 

        return ss.str();
    }

};

#endif//PLAYER_HPP
