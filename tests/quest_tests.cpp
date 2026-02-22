#include "Quest.hpp"
#include "TestSupport.hpp"
#include "script/js/ScriptedThing.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <cstdlib>
#include <doctest/doctest.h>

namespace
{
void InitScriptVMsForTests()
{
    static bool initialized = false;
    if (initialized)
        return;

    setenv("GAUZARBEIT_SCRIPT_ROOT", "tests/scripts", 1);
    ScriptedThing_Lua::Init();
    ScriptedThing_JS::Init();
    initialized = true;
}
} // namespace

TEST_CASE("ScriptedQuest loads Lua quest tasks")
{
    InitScriptVMsForTests();

    auto quest = ScriptedQuest("TestQuest");
    REQUIRE(quest != nullptr);
    REQUIRE(quest->_tasker != nullptr);
    REQUIRE(quest->tasker()->tasks.size() == 2);
    CHECK(quest->tasker()->tasks[0].description == "Talk to TestTalker");
    CHECK(quest->tasker()->tasks[1].description == "Open TestChest");
}

TEST_CASE("ScriptedQuest Lua reward hook runs")
{
    InitScriptVMsForTests();

    auto completer = MakeBasicThing("Completer");
    auto quest = ScriptedQuest("TestQuest");
    REQUIRE(quest != nullptr);

    quest->tasker()->doReward(quest, completer);

    auto net = std::dynamic_pointer_cast<TestNetworked>(completer->networked());
    REQUIRE(net != nullptr);
    CHECK(net->response().find("TestQuest complete!") != std::string::npos);
}

TEST_CASE("ScriptedQuest dispatches JavaScript quest implementation")
{
    InitScriptVMsForTests();

    auto quest = ScriptedQuest("TestQuestJS");
    REQUIRE(quest != nullptr);
    REQUIRE(quest->_tasker != nullptr);
    REQUIRE(quest->tasker()->tasks.size() == 2);
    CHECK(quest->tasker()->tasks[0].description == "Collect JS token");
    CHECK(quest->tasker()->tasks[1].description == "Return to JS guide");
}
