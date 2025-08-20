#ifndef QUEST_HPP
#define QUEST_HPP

#include "script/ScriptedThing.hpp"
#include <memory>
#include <string>

class ScriptedQuest : public ScriptedThing
{
  public:
    ScriptedQuest(const std::string& name);
};

#endif // QUEST_HPP
