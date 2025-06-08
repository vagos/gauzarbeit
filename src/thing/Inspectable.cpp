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

    if (!owner->physical()->inventory.empty())
    {
        inspect << "Inventory: \n";
        inspect << VerticalListString(owner->physical()->inventory, '-');
    }

    if (!owner->achiever()->quests.empty())
    {
        inspect << "Quests: \n";
        inspect << VerticalListString(owner->achiever()->quests, '-',
                                      [](const auto& t) { return t->name; });
    }

    if (!owner->tasker()->tasks.empty())
    {
        inspect << "Tasks: \n";

        inspect << VerticalListString(owner->tasker()->tasks, '-',
                                      [](const Tasker::Task& task)
                                      {
                                          std::stringstream ss;
                                          ss << '[' << (task.tick ? 'x' : ' ') << ']' << ' ';
                                          ss << task.description;
                                          return ss.str();
                                      });
    }

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
