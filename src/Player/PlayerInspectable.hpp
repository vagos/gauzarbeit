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
                << "Level: " << owner -> achiever() -> getLevel() << '\n'
                << "HP: " << BarString(owner -> attackable() -> current_health/owner -> attackable() -> max_health) << "\n\n\n";

        if (owner == inspector)
        {
            inspect << CenteredString("---") << "\n\n";

            if (!owner -> physical() -> inventory.empty())
            {
                inspect << "Inventory: \n";
                inspect << BlockListString(owner -> physical() -> inventory, '-') << "\n\n";
            }

            if (!owner -> physical() -> equipment.empty())
            {
                inspect << "Equipment: \n";
                inspect << VerticalListString(owner -> physical() -> equipment, '-') << "\n\n";
            }
            
            if (!owner -> achiever() -> quests.empty())
            {
                inspect << "Quests: \n";
                inspect << VerticalListString(owner -> achiever() -> quests, '-', 
                        [](const Thing& t) {return t.name;});
            }
        }

        return HeaderString( inspect.str(), owner -> name ); 
    }


};

#endif
