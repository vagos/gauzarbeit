#ifndef QUEST_HPP
#define QUEST_HPP

#include <memory>
#include <string>

#include "script/ScriptedThing.hpp"

class ScriptedQuest : public ScriptedThing
{
  public:
	ScriptedQuest(const std::string& name);
};

#endif // QUEST_HPP
