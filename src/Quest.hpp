#ifndef QUEST_HPP
#define QUEST_HPP

#include <memory>
#include <string>

#include "Physical.hpp"
#include "Script/ScriptedThing.hpp"
#include "Thing.hpp"


class ScriptedQuest : public ScriptedThing
{
public:
    ScriptedQuest(const std::string& name):
        ScriptedThing(name, "./Scripts/Quests/")
    {

    }

};

#endif//QUEST_HPP
