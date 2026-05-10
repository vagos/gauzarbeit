#include "Exceptions.hpp"
#include "Room.hpp"
#include "player/CommandParser.hpp"
#include "player/Player.hpp"
#include "script/lua/ScriptedThing.hpp"
#include "TestSupport.hpp"
#include <doctest/doctest.h>

TEST_CASE("PlayerNotifier parses commands into events")
{
    Player::setPlayerCommands();
    auto event = CommandParser::Parse("Say Hello there friend");

    CHECK_EQ(event.verb, "say");
    CHECK_EQ(event.target, "Hello");
    CHECK_EQ(event.object, "there");
    CHECK_EQ(event.extra, "friend");
    CHECK_EQ(event.type, Event::Type::Say);
}

TEST_CASE("PlayerNotifier rejects unknown commands")
{
    Player::setPlayerCommands();
    CHECK_THROWS_AS(CommandParser::Parse("nonsense"), InvalidCommand);
}

TEST_CASE("PlayerNotifier handles movement targets")
{
    Player::setPlayerCommands();
    auto event = CommandParser::Parse("move left");

    CHECK_EQ(event.verb, "move");
    CHECK_EQ(event.target, "left");
    CHECK_EQ(event.type, Event::Type::Move);
}

TEST_CASE("PlayerNotifier parses admin eval commands")
{
    Player::setPlayerCommands();
    auto event = CommandParser::Parse("eval lua return 1 + 2");

    CHECK_EQ(event.verb, "eval");
    CHECK_EQ(event.target, "lua");
    CHECK_EQ(event.object, "return");
    CHECK_EQ(event.extra, "1 + 2");
    CHECK_EQ(event.type, Event::Type::Custom);
}

TEST_CASE("Script eval helpers can run Lua code")
{
    InitScriptVMsForTests();

    CHECK(ScriptedThing_Lua::Eval("return 1 + 2") == "3");
}

TEST_CASE("Admin eval can spawn objects in the current room")
{
    InitScriptVMsForTests();

    auto room = Room::get(42, 42);
    auto admin = MakeBasicThing("Admin");
    admin->physical()->current_room = room;

    const auto initial_count = room->things.size();

    CHECK(ScriptedThing_Lua::Eval("local t = Gauzarbeit.Spawn(self:getRoom().x, self:getRoom().y, 'TestDummy')\n"
                                  "return t:getName()",
                                  admin.get()) == "TestDummy");

    CHECK(room->things.size() == initial_count + 1);
    CHECK(room->getThing("TestDummy") != nullptr);
    CHECK(room->getThing("TestDummy")->script_language == Thing::ScriptLanguage::Lua);
}
