#ifndef PLAYER_INSPECTABLE_HPP
#define PLAYER_INSPECTABLE_HPP

#include "Helpers.hpp"
#include "thing/Inspectable.hpp"
#include "thing/Thing.hpp"

class PlayerInspectable : public Inspectable
{
  public:
    const std::string onInspect(const std::shared_ptr<Thing>& owner,
                                const std::shared_ptr<Thing>& inspector) override
    {
        std::stringstream inspect;

        inspect << BlockListStringSimple(owner->achiever()->extra_stats, 0,
                                         [](auto& p)
                                         { return p.first + ": " + p.second->getName(); })
                << '\n';

        inspect << "Level: " << owner->achiever()->getLevel() << '\n'
                << "HP: "
                << BarString(owner->attackable()->current_health / owner->attackable()->max_health)
                << "\n\n\n";

        if (owner == inspector)
        {
            inspect << CenteredString("---") << "\n";

            if (!owner->physical()->inventory.empty())
            {
                inspect << "Inventory: \n";
                inspect << BlockListString(owner->physical()->inventory, '-',
                                           [](const std::shared_ptr<Thing>& t)
                                           { return t->inspectable()->getName(t); })
                        << "\n\n";
            }

            if (!owner->physical()->equipment.empty())
            {
                inspect << "Equipment: \n";
                inspect << VerticalListString(owner->physical()->equipment, '-',
                                              [](const std::shared_ptr<Thing>& t)
                                              { return t->inspectable()->getName(t); })
                        << "\n\n";
            }

            if (!owner->achiever()->quests.empty())
            {
                inspect << "Quests: \n";
                inspect << VerticalListString(owner->achiever()->quests, '-',
                                              [](const std::shared_ptr<Thing>& t)
                                              { return t->inspectable()->getName(t); });
            }
        }

        return HeaderString(inspect.str(), getName(owner));
    }
};

#endif
