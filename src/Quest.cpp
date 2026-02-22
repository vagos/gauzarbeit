#include "Quest.hpp"
#include "script/ScriptedThing.hpp"

std::shared_ptr<Thing> ScriptedQuest(const std::string& q_name, const std::string& script_dir)
{
    return ScriptedThing(q_name, script_dir);
}
