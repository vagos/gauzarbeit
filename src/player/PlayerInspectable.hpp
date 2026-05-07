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

        inspect << BlockListString(owner->achiever()->extra_stats, 0,
                                   [](auto& p) { return p.first + ": " + p.second->getName(); })
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

            if (!owner->tasker()->tasks.empty())
            {
                inspect << "Tasks: \n";
                inspect << VerticalListString(owner->tasker()->tasks, '-',
                                              [owner](const std::unique_ptr<Tasker::Task>& task)
                                              {
                                                  std::stringstream ss;
                                                  ss << '[' << (task->tick ? 'x' : ' ') << ']'
                                                     << ' ';
                                                  ss << owner->tasker()->formatTask(*task);
                                                  return ss.str();
                                              });
            }
        }

        return HeaderString(inspect.str(), getName(owner));
    }
};

#endif
