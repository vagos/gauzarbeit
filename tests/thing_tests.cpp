#include "Room.hpp"
#include "TestSupport.hpp"
#include "World.hpp"
#include "player/Player.hpp"
#include "script/ScriptedThing.hpp"
#include "script/lua/LuaHelpers.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <doctest/doctest.h>
#include <filesystem>
#include <lua.hpp>

TEST_CASE("Physical pickup and drop move items between room and inventory")
{
    auto room = std::make_shared<Room>(0, 0);
    auto owner = MakeBasicThing("Owner");
    owner->physical()->current_room = room;
    room->addThing(owner);

    auto item = MakeBasicThing("Item");
    room->addThing(item);

    owner->physical()->pickupItem(item);
    CHECK(owner->physical()->hasItem(item));
    CHECK(room->getThing("Item") == nullptr);

    owner->physical()->dropItem(item);
    CHECK(!owner->physical()->hasItem(item));
    CHECK(room->getThing("Item") == item);
}

TEST_CASE("Physical pickup respects immovable items")
{
    auto room = std::make_shared<Room>(1, 1);
    auto owner = MakeBasicThing("Owner");
    owner->physical()->current_room = room;
    room->addThing(owner);

    auto item = MakeBasicThing("Anvil");
    item->physical()->movable = false;
    room->addThing(item);

    owner->physical()->pickupItem(item);
    CHECK(!owner->physical()->hasItem(item));
    CHECK(room->getThing("Anvil") == item);
}

TEST_CASE("Physical give moves items between inventories")
{
    auto giver = MakeBasicThing("Giver");
    auto receiver = MakeBasicThing("Receiver");
    auto item = MakeBasicThing("Ring");

    giver->physical()->gainItem(item);
    giver->physical()->giveItem(receiver, item);

    CHECK(!giver->physical()->hasItem(item));
    CHECK(receiver->physical()->hasItem(item));
}

TEST_CASE("Physical doMove updates the owning room")
{
    Room::mapRooms.clear();

    auto room = std::make_shared<Room>(0, 0);
    auto mover = MakeBasicThing("Mover");
    mover->physical()->current_room = room;
    room->addThing(mover);

    mover->physical()->doMove(mover, 5, 6);

    CHECK(mover->physical()->current_room->x == 5);
    CHECK(mover->physical()->current_room->y == 6);
    CHECK(room->getThing("Mover") == nullptr);
    CHECK(mover->physical()->current_room->getThing("Mover") == mover);
}

TEST_CASE("Attackable doAttack kills and removes target")
{
    World world;
    auto room = std::make_shared<Room>(0, 0);
    auto attacker = MakeBasicThing("Attacker");
    auto target = MakeBasicThing("Target");

    attacker->physical()->current_room = room;
    target->physical()->current_room = room;
    room->addThing(attacker);
    room->addThing(target);

    target->attackable()->setMaxHealth(1);
    attacker->attackable()->doAttack(attacker, target);

    CHECK(target->attackable()->is_alive());
    CHECK(target->attackable()->current_health == doctest::Approx(1.0));
    CHECK(target->notifier()->event.type == Event::Type::Attacked);
    CHECK(target->notifier()->event.target == "Attacker");

    target->thinker()->doThink(target, world);

    CHECK(!target->attackable()->is_alive());
    CHECK(target->physical()->current_room == nullptr);
    CHECK(room->getThing("Target") == nullptr);
}

TEST_CASE("Tasker completes tasks and grants rewards")
{
    auto owner = MakeBasicThing("Player");

    owner->tasker()->addTask("Find cheese");
    owner->tasker()->addTask("Return to chef");
    owner->tasker()->tickTask(0);
    owner->tasker()->tickTask(1);

    CHECK(owner->tasker()->rewardTask(owner, "Find cheese"));
    CHECK(owner->tasker()->rewardTask(owner, "Return to chef"));

    CHECK(owner->achiever()->getXP() == 25);
    CHECK(owner->tasker()->tasks.empty());
    CHECK(owner->tasker()->done_tasks.size() == 2);
    CHECK(owner->tasker()->done_tasks.front()->description == "Find cheese");
}

TEST_CASE("Tasker completion and difficulty tracking")
{
    Tasker tasker;
    CHECK(tasker.getDifficulty() == 0);
    CHECK(tasker.isCompleted());

    auto task_index = tasker.addTask("Collect herbs");
    CHECK(tasker.getDifficulty() == 1);
    CHECK(!tasker.isCompleted());

    tasker.tickTask(task_index);
    CHECK(tasker.isCompleted());
}

TEST_CASE("Room routes players into players list when added as a thing")
{
    auto room = std::make_shared<Room>(0, 0);
    auto player = std::make_shared<Player>();

    room->addThing(player);

    CHECK(std::find(room->players.begin(), room->players.end(), player) != room->players.end());
    CHECK(std::find(room->things.begin(), room->things.end(), player) == room->things.end());
}

TEST_CASE("Room speech broadcasts clear the active line")
{
    auto room = std::make_shared<Room>(1, 2);
    auto speaker = MakeBasicThing("Rat");
    auto listener = MakeBasicThing("Listener");

    speaker->is_player = true;
    listener->is_player = true;
    speaker->physical()->current_room = room;
    listener->physical()->current_room = room;
    room->addThing(speaker);
    room->addThing(listener);

    room->onSay(speaker, "Squeak");

    auto listener_net = std::dynamic_pointer_cast<TestNetworked>(listener->networked());
    REQUIRE(listener_net != nullptr);
    CHECK(listener_net->response().find(std::string(PromptReset) + "Rat: Squeak") !=
          std::string::npos);
}

TEST_CASE("Lua index/newindex uses Lua registry table for Lua scripted things")
{
    InitScriptVMsForTests();

    auto lua_thing = std::make_shared<ScriptedThing_Lua>("TestDummy");
    lua_State* L = ScriptedThing_Lua::L;

    lua_settop(L, 0);
    lua_pushlightuserdata(L, lua_thing.get());
    lua_setglobal(L, "__test_thing");

    CheckLua(L, luaL_dostring(L, "__test_thing.test_flag = 99"));
    CheckLua(L, luaL_dostring(L, "return __test_thing.test_flag"));
    REQUIRE(lua_isnumber(L, -1));
    CHECK(lua_tointeger(L, -1) == 99);
    lua_settop(L, 0);
}

TEST_CASE("Lua index skips Lua-only field lookup for non-Lua things")
{
    InitScriptVMsForTests();

    Thing js_thing("TestDummy");
    js_thing.script_language = Thing::ScriptLanguage::JS;

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, &js_thing);
    lua_setglobal(L, "__test_thing");

    CheckLua(L, luaL_dostring(L, "return __test_thing.counter"));
    CHECK(lua_isnil(L, -1));
    lua_settop(L, 0);
}

TEST_CASE("Lua GetRoom with no arguments returns a sorted room list")
{
    InitScriptVMsForTests();

    Room::mapRooms.clear();

    auto room_a = Room::get(2, 1);
    room_a->name = "Alpha";
    auto room_b = Room::get(1, 3);
    room_b->name = "Beta";

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);

    CheckLua(L, luaL_dostring(L, "local rooms = Gauzarbeit.GetRoom()\n"
                                 "return #rooms, rooms[1].x, rooms[1].y, rooms[1].name, "
                                 "rooms[2].x, rooms[2].y, rooms[2].name"));

    REQUIRE(lua_gettop(L) == 7);
    CHECK(lua_tointeger(L, 1) == 2);
    CHECK(lua_tointeger(L, 2) == 2);
    CHECK(lua_tointeger(L, 3) == 1);
    CHECK(std::string(lua_tostring(L, 4)) == "Alpha");
    CHECK(lua_tointeger(L, 5) == 1);
    CHECK(lua_tointeger(L, 6) == 3);
    CHECK(std::string(lua_tostring(L, 7)) == "Beta");

    lua_settop(L, 0);
}

TEST_CASE("Lua can seed WithChance")
{
    InitScriptVMsForTests();

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);

    CheckLua(L, luaL_dostring(L, "Gauzarbeit.SeedRNG(12345)\n"
                                 "local first = Gauzarbeit.WithChance(0.5)\n"
                                 "local second = Gauzarbeit.WithChance(0.5)\n"
                                 "Gauzarbeit.SeedRNG(12345)\n"
                                 "return first == Gauzarbeit.WithChance(0.5), "
                                 "second == Gauzarbeit.WithChance(0.5)"));

    REQUIRE(lua_gettop(L) == 2);
    CHECK(lua_toboolean(L, 1));
    CHECK(lua_toboolean(L, 2));
    lua_settop(L, 0);
}

TEST_CASE("Lua room userdata exposes coordinates and name")
{
    InitScriptVMsForTests();

    Room::mapRooms.clear();

    auto room = Room::get(4, -2);
    room->name = "Crossroads";
    auto lua_thing = std::make_shared<ScriptedThing_Lua>("TestDummy");
    lua_thing->physical()->current_room = room;
    room->addThing(lua_thing);

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, lua_thing.get());
    lua_setglobal(L, "__lua");

    CheckLua(L, luaL_dostring(L, "local room = __lua:getRoom()\n"
                                 "return room.x, room.y, room.name"));

    REQUIRE(lua_gettop(L) == 3);
    CHECK(lua_tointeger(L, 1) == 4);
    CHECK(lua_tointeger(L, 2) == -2);
    CHECK(std::string(lua_tostring(L, 3)) == "Crossroads");
    lua_settop(L, 0);
}

TEST_CASE("Lua doLater runs callbacks after the scheduled time")
{
    InitScriptVMsForTests();

    auto lua_thing = std::make_shared<ScriptedThing_Lua>("TestDummy");
    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, lua_thing.get());
    lua_setglobal(L, "__lua");

    CheckLua(L, luaL_dostring(L, "Gauzarbeit.__later_count = 0\n"
                                 "__lua:doLater(function()\n"
                                 "    Gauzarbeit.__later_count = Gauzarbeit.__later_count + 1\n"
                                 "end, 5)"));

    ScriptedThing_Lua::RunScheduledCallbacks(4.9);
    CheckLua(L, luaL_dostring(L, "return Gauzarbeit.__later_count"));
    REQUIRE(lua_isnumber(L, -1));
    CHECK(lua_tointeger(L, -1) == 0);
    lua_settop(L, 0);

    ScriptedThing_Lua::RunScheduledCallbacks(5.0);
    CheckLua(L, luaL_dostring(L, "return Gauzarbeit.__later_count"));
    REQUIRE(lua_isnumber(L, -1));
    CHECK(lua_tointeger(L, -1) == 1);
    lua_settop(L, 0);
}

TEST_CASE("Lua thing doLater method runs callbacks during world update")
{
    InitScriptVMsForTests();

    World world;
    auto lua_thing = std::make_shared<ScriptedThing_Lua>("TestDummy");
    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, lua_thing.get());
    lua_setglobal(L, "__lua");

    CheckLua(L, luaL_dostring(L, "__lua.later_count = 0\n"
                                 "__lua:doLater(function()\n"
                                 "    __lua.later_count = __lua.later_count + 1\n"
                                 "end, 0)"));

    world.doUpdate();

    CheckLua(L, luaL_dostring(L, "return __lua.later_count"));
    REQUIRE(lua_isnumber(L, -1));
    CHECK(lua_tointeger(L, -1) == 1);
    lua_settop(L, 0);
}

TEST_CASE("Lua thing can index a JS thing via Lua __index")
{
    InitScriptVMsForTests();

    auto room = std::make_shared<Room>(0, 0);
    auto lua_thing = std::make_shared<ScriptedThing_Lua>("TestDummy");
    auto js_thing = ScriptedThing("TestTalker");

    REQUIRE(lua_thing != nullptr);
    REQUIRE(js_thing != nullptr);

    lua_thing->physical()->current_room = room;
    js_thing->physical()->current_room = room;
    room->addThing(lua_thing);
    room->addThing(js_thing);

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, lua_thing.get());
    lua_setglobal(L, "__lua");

    CheckLua(L, luaL_dostring(L, "local other = __lua:getThing('TestTalker')\n"
                                 "return other ~= nil and other.getName ~= nil and "
                                 "other:getName() == 'TestTalker' and other.counter == nil"));
    CHECK(lua_toboolean(L, -1) == 1);
    lua_settop(L, 0);
}

TEST_CASE("Production Lua things initialize and inspect")
{
    InitScriptVMsForTests();

    const auto things_dir = std::filesystem::absolute("ext/things");
    auto inspector = MakeBasicThing("Inspector");

    for (const auto& entry : std::filesystem::directory_iterator(things_dir))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".lua")
            continue;

        const auto thing_name = entry.path().stem().string();
        CAPTURE(thing_name);

        auto thing = std::make_shared<ScriptedThing_Lua>(thing_name, things_dir.string());
        REQUIRE(thing != nullptr);
        CHECK(thing->script_language == Thing::ScriptLanguage::Lua);
        CHECK(!thing->inspectable()->onInspect(thing, inspector).empty());
    }
}

TEST_CASE("PokeBall catches and releases a room thing")
{
    InitScriptVMsForTests();

    Room::mapRooms.clear();

    auto room = Room::get(12, 34);
    auto player = MakeBasicThing("Player");
    player->physical()->current_room = room;
    room->addThing(player);

    auto poke_ball = std::make_shared<ScriptedThing_Lua>(
        "PokeBall", std::filesystem::absolute("ext/things").string());
    poke_ball->physical()->current_room = room;
    room->addThing(poke_ball);

    auto target = std::make_shared<ScriptedThing_Lua>(
        "Cheese", std::filesystem::absolute("ext/things").string());
    target->physical()->current_room = room;
    room->addThing(target);

    player->notifier()->event.object = "Cheese";
    poke_ball->usable()->onUse(poke_ball, player);

    CHECK(room->getThing("Cheese") == nullptr);
    CHECK(poke_ball->physical()->getItem("Cheese") != nullptr);
    CHECK(poke_ball->inspectable()->onInspect(poke_ball, player).find("Cheese") !=
          std::string::npos);

    poke_ball->usable()->onUse(poke_ball, player);

    CHECK(room->getThing("Cheese") != nullptr);
    CHECK(poke_ball->physical()->getItem("Cheese") == nullptr);
}

TEST_CASE("Room serialization saves and restores non-player things")
{
    InitScriptVMsForTests();

    const int x = 987;
    const int y = 654;
    const std::filesystem::path room_db = "./db/rooms/987_654";

    std::filesystem::remove(room_db);
    Room::mapRooms.clear();

    auto room = Room::get(x, y);
    room->name = "PersistedRoom";

    auto dummy = ScriptedThing("TestDummy");
    REQUIRE(dummy != nullptr);
    dummy->physical()->current_room = room;
    room->addThing(dummy);
    room->networked()->doDatabaseStore(room);

    Room::mapRooms.clear();

    auto loaded = Room::get(x, y);
    REQUIRE(loaded != nullptr);
    CHECK(loaded->name == "PersistedRoom");
    CHECK(loaded->getThing("TestDummy") != nullptr);

    std::filesystem::remove(room_db);
}

TEST_CASE("Room serialization preserves scripted thing payloads")
{
    InitScriptVMsForTests();

    const int x = 989;
    const int y = 656;
    const std::filesystem::path room_db = "./db/rooms/989_656";

    std::filesystem::remove(room_db);
    Room::mapRooms.clear();

    auto room = Room::get(x, y);
    room->name = "TableRoom";

    auto table = ScriptedThing("Table");
    REQUIRE(table != nullptr);
    table->physical()->doMove(table, room);

    auto item = ScriptedThing("TestDummy");
    REQUIRE(item != nullptr);
    item->physical()->doMove(item, room);
    table->physical()->pickupItem(item);
    CHECK(table->physical()->hasItem(item));
    CHECK(table->physical()->inventory.size() == 1);
    CHECK(room->getThing("TestDummy") == nullptr);

    auto saved_table = table->networked()->doDatabaseSave(table);
    CHECK(saved_table == "1\nTestDummy\n");

    room->networked()->doDatabaseStore(room);

    Room::mapRooms.clear();

    auto loaded = Room::get(x, y);
    REQUIRE(loaded != nullptr);
    auto loaded_table = loaded->getThing("Table");
    REQUIRE(loaded_table != nullptr);
    CHECK(loaded_table->physical()->getItem("TestDummy") != nullptr);

    std::filesystem::remove(room_db);
}

TEST_CASE("SetRoom loads persisted rooms instead of re-running room init")
{
    InitScriptVMsForTests();

    const int x = 988;
    const int y = 655;
    const std::filesystem::path room_db = "./db/rooms/988_655";

    std::filesystem::remove(room_db);
    Room::mapRooms.clear();

    auto room = Room::get(x, y);
    room->name = "PersistedKitchen";

    auto dummy = ScriptedThing("TestDummy");
    REQUIRE(dummy != nullptr);
    dummy->physical()->current_room = room;
    room->addThing(dummy);
    room->networked()->doDatabaseStore(room);

    Room::mapRooms.clear();

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    CheckLua(L, luaL_dostring(L, "return Gauzarbeit.SetRoom('Kitchen', 988, 655)"));
    lua_settop(L, 0);

    auto loaded = Room::get(x, y);
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getThing("TestDummy") != nullptr);
    CHECK(loaded->getThing("WanderingCook") == nullptr);
    CHECK(loaded->getThing("Rat") == nullptr);

    std::filesystem::remove(room_db);
}
