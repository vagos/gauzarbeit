#include "thing/Inspectable.hpp"
#include "Helpers.hpp"
#include "thing/Thing.hpp"
#include <sstream>

Inspectable::Inspectable() {}

const std::string Inspectable::onInspect(const std::shared_ptr<Thing>& owner,
                                         const std::shared_ptr<Thing>& inspector)
{
    std::stringstream inspect;
    inspect << "Name: " << owner->name << "\n\n\n";
    inspect << CenteredString("---") << "\n\n";

    if (owner != inspector)
        goto Return;

    if (!owner->physical()->inventory.empty())
    {
        inspect << "Inventory: \n";
        inspect << VerticalListString(owner->physical()->inventory, '-');
    }

    if (owner->_achiever && !owner->achiever()->extra_stats.empty())
    {
        inspect << "Stats: \n";
        for (const auto& [name, stat] : owner->achiever()->extra_stats)
        {
            if (!stat)
                continue;

            inspect << "- " << name << ": " << stat->value << '\n';
        }
    }

    if (!owner->tasker()->tasks.empty())
    {
        inspect << "Tasks: \n";
        inspect << VerticalListString(owner->tasker()->tasks, '-',
                                      [owner](const std::unique_ptr<Tasker::Task>& task)
                                      {
                                          std::stringstream ss;
                                          ss << '[' << (task->tick ? 'x' : ' ') << ']' << ' ';
                                          ss << owner->tasker()->formatTask(*task);
                                          return ss.str();
                                      });
    }

    Return:
    return HeaderString(inspect.str(), owner->name);
}

const std::string Inspectable::onHelp(std::shared_ptr<Thing> owner,
                                      std::shared_ptr<Thing> inspector)
{
    return "";
}

const std::string Inspectable::getName(const std::shared_ptr<Thing>& owner)
{
    return owner->name;
}
