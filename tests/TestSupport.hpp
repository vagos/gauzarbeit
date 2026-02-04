#pragma once

#include "thing/Attackable.hpp"
#include "thing/Achiever.hpp"
#include "thing/Inspectable.hpp"
#include "thing/Networked.hpp"
#include "thing/Physical.hpp"
#include "thing/Talker.hpp"
#include "thing/Thing.hpp"
#include <memory>
#include <string>

struct TestNetworked : public Networked
{
    std::string response() const { return streamResponse.str(); }
    void clearResponse() { streamResponse.str(std::string()); }
};

inline std::shared_ptr<Thing> MakeBasicThing(const std::string& name)
{
    auto thing = std::make_shared<Thing>(name);
    thing->_physical = std::make_shared<Physical>();
    thing->_attackable = std::make_shared<Attackable>();
    thing->_notifier = std::make_shared<Notifier>();
    thing->_achiever = std::make_shared<Achiever>();
    thing->_tasker = std::make_shared<Tasker>();
    thing->_inspectable = std::make_shared<Inspectable>();
    thing->_talker = std::make_shared<Talker>();
    thing->_networked = std::make_shared<TestNetworked>();
    thing->_thinker = std::make_shared<Thinker>();
    return thing;
}
