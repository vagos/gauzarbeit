#pragma once

#include "thing/Thing.hpp"
#include <memory>

namespace script
{
// Language-agnostic scripted thing contract.
class ScriptedThing : public Thing
{
  public:
    explicit ScriptedThing(const std::string& name) : Thing(name) {}
    ~ScriptedThing() = default;
};
} // namespace script

std::shared_ptr<Thing> ScriptedThing(const std::string& name,
                                     const std::string& script_dir = "./ext/things/");
