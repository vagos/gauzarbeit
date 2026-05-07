#include "Room.hpp"
#include "TestSupport.hpp"
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

TEST_CASE("Physical give and equip move items between inventories")
{
    auto giver = MakeBasicThing("Giver");
    auto receiver = MakeBasicThing("Receiver");
    auto item = MakeBasicThing("Ring");

    giver->physical()->gainItem(item);
    giver->physical()->giveItem(receiver, item);

    CHECK(!giver->physical()->hasItem(item));
    CHECK(receiver->physical()->hasItem(item));

    auto gear = MakeBasicThing("Helmet");
    giver->physical()->gainItem(gear);
    giver->physical()->equipItem(gear);

    CHECK(!giver->physical()->hasItem(gear));
    CHECK(giver->physical()->equipment.size() == 1);
    CHECK(giver->physical()->equipment.front() == gear);
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
    auto room = std::make_shared<Room>(0, 0);
    auto attacker = MakeBasicThing("Attacker");
    auto target = MakeBasicThing("Target");

    attacker->physical()->current_room = room;
    target->physical()->current_room = room;
    room->addThing(attacker);
    room->addThing(target);

    target->attackable()->setMaxHealth(1);
    attacker->attackable()->dmg = 1;

    attacker->attackable()->doAttack(attacker, target);

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

    owner->tasker()->doUpdate(owner);

    CHECK(owner->achiever()->getXP() == 25);
    CHECK(owner->tasker()->tasks.empty());
    CHECK(owner->tasker()->completed_tasks.size() == 2);
    CHECK(owner->tasker()->completed_tasks.front().description == "Find cheese");
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

TEST_CASE("Lua index skips Lua-only field lookup for JS scripted things")
{
    InitScriptVMsForTests();

    auto js_thing = ScriptedThing("TestDummy");
    REQUIRE(js_thing != nullptr);

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    lua_pushlightuserdata(L, js_thing.get());
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
