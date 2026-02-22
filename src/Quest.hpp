#ifndef QUEST_HPP
#define QUEST_HPP

#include "thing/Thing.hpp"
#include <memory>
#include <string>

std::shared_ptr<Thing> ScriptedQuest(const std::string& name,
                                     const std::string& script_dir = "./scripts/quests/");

#endif // QUEST_HPP
