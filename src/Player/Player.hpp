#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../Thing.hpp"
#include <memory>
#include <sstream>
#include <unordered_map>


class Player : public Thing
{
public:
    Player(); 

    const std::string getInfo() const override
    {
        std::stringstream ss;
        
        ss << "(Player) " << "[" << _networked -> getID() << "]"; 

        return ss.str();
    }

    static std::unordered_map<std::string, Event::Type> playerCommands;

    static void setPlayerCommands();
};




#endif//PLAYER_HPP
