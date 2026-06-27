#include "Room.hpp"
#include "TestSupport.hpp"
#include "World.hpp"
#include "player/Player.hpp"
#include "player/PlayerNotifier.hpp"
#include "script/ScriptedThing.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <doctest/doctest.h>

TEST_CASE("Tasker gives task objects")
{
    auto giver = MakeBasicThing("Giver");
    auto receiver = MakeBasicThing("Receiver");

    CHECK(giver->tasker()->giveTask(receiver, std::make_unique<Tasker::Task>("Collect herbs")));

    REQUIRE(receiver->tasker()->tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.front()->description == "Collect herbs");
    CHECK(!receiver->tasker()->tasks.front()->tick);
}

TEST_CASE("Lua scripted tasker gives and ticks receiver tasks")
{
    InitScriptVMsForTests();
    Event event{};

    auto receiver = MakeBasicThing("Receiver");
    auto giver = std::make_shared<ScriptedThing_Lua>("TaskGiver");
    auto ticker = std::make_shared<ScriptedThing_Lua>("TaskTicker");

    giver->talker()->onTalk(giver, receiver, event);
    REQUIRE(receiver->tasker()->tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.front()->description == "Collect herbs");

    ticker->talker()->onTalk(ticker, receiver, event);
    CHECK(receiver->tasker()->tasks.front()->tick);
}

TEST_CASE("Wick_McWelcome gives rat task")
{
    InitScriptVMsForTests();
    Event event(Event::Type::Invalid, "", "", "rats");

    auto receiver = MakeBasicThing("Receiver");
    auto welcome_man = std::make_shared<ScriptedThing_Lua>("Wick_McWelcome");

    welcome_man->talker()->onTalk(welcome_man, receiver, event);

    REQUIRE(receiver->tasker()->tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.front()->description == "Kill 5 Rats.");
    CHECK(receiver->tasker()->formatTask(*receiver->tasker()->tasks.front()) ==
          "Kill 5 Rats. 0/5 Rats killed.");

    welcome_man->talker()->onTalk(welcome_man, receiver, event);
    CHECK(receiver->tasker()->tasks.size() == 1);
}

TEST_CASE("Wick_McWelcome rat task tracks kill progress")
{
    InitScriptVMsForTests();
    Event talk_event(Event::Type::Invalid, "", "", "rats");
    Event kill_event(Event::Type::Kill);

    auto receiver = MakeBasicThing("Receiver");
    auto welcome_man = std::make_shared<ScriptedThing_Lua>("Wick_McWelcome");
    auto rat = MakeBasicThing("Rat");

    welcome_man->talker()->onTalk(welcome_man, receiver, talk_event);
    REQUIRE(receiver->tasker()->tasks.size() == 1);

    for (int i = 0; i < 4; ++i)
        receiver->tasker()->onNotify(receiver, receiver, kill_event, rat);

    REQUIRE(receiver->tasker()->tasks.size() == 1);
    CHECK(!receiver->tasker()->tasks.front()->tick);
    CHECK(receiver->tasker()->formatTask(*receiver->tasker()->tasks.front()) ==
          "Kill 5 Rats. 4/5 Rats killed.");

    receiver->tasker()->onNotify(receiver, receiver, kill_event, rat);
    CHECK(receiver->tasker()->tasks.front()->tick);
}

TEST_CASE("Player kill notifications complete rat task before returning to Wick_McWelcome")
{
    InitScriptVMsForTests();
    Event talk_event(Event::Type::Invalid, "", "", "rats");
    Event kill_event(Event::Type::Kill);

    World world;
    auto room = std::make_shared<Room>(0, 0);
    auto receiver = std::make_shared<Player>();
    receiver->name = "Receiver";
    receiver->physical()->current_room = room;
    room->addThing(receiver);
    world.addPlayer(receiver);

    auto welcome_man = std::make_shared<ScriptedThing_Lua>("Wick_McWelcome");
    auto rat = MakeBasicThing("Rat");
    rat->physical()->current_room = room;
    room->addThing(rat);

    welcome_man->talker()->onTalk(welcome_man, receiver, talk_event);
    REQUIRE(receiver->tasker()->tasks.size() == 1);

    for (int i = 0; i < 5; ++i)
        receiver->notifier()->doNotify(receiver, kill_event, rat);

    REQUIRE(receiver->tasker()->tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.front()->tick);
    world.doUpdate();

    CHECK(receiver->tasker()->tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.front()->tick);

    welcome_man->talker()->onTalk(welcome_man, receiver, talk_event);
    CHECK(receiver->tasker()->done_tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.empty());
    CHECK(receiver->physical()->inventory.size() == 10);
}

TEST_CASE("Wick_McWelcome rewards completed rat task once")
{
    InitScriptVMsForTests();
    Event event(Event::Type::Invalid, "", "", "rats");

    auto receiver = MakeBasicThing("Receiver");
    auto welcome_man = std::make_shared<ScriptedThing_Lua>("Wick_McWelcome");

    receiver->tasker()->addTask("Kill 5 Rats.");
    receiver->tasker()->tickTask("Kill 5 Rats.");
    receiver->tasker()->doUpdate(receiver);

    welcome_man->talker()->onTalk(welcome_man, receiver, event);

    CHECK(receiver->tasker()->done_tasks.size() == 1);
    CHECK(receiver->tasker()->tasks.empty());
    CHECK(receiver->physical()->inventory.size() == 10);
    for (const auto& item : receiver->physical()->inventory)
        CHECK(item->name == "Winston");

    welcome_man->talker()->onTalk(welcome_man, receiver, event);
    CHECK(receiver->physical()->inventory.size() == 10);
    CHECK(receiver->tasker()->tasks.empty());
    CHECK(receiver->tasker()->done_tasks.size() == 1);
}
