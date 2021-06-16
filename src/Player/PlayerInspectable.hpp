#ifndef PLAYER_INSPECTABLE_HPP
#define PLAYER_INSPECTABLE_HPP

#include "../Thing.hpp"

#include "../Helpers.hpp"


class PlayerInspectable : public Inspectable
{
public:

    const std::string onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector) override
    {
        std::stringstream inspect; 

        inspect << "Name: " << owner -> name << '\n'
                << "Level: " << owner -> achiever -> getLevel() << '\n'
                << "HP: " << owner -> attackable -> current_health << '\n';

//        inspect << VerticalListString(owner -> physical -> inventory, '&');

        return HeaderString( inspect.str(), owner -> name ); 
    }


};

#endif
