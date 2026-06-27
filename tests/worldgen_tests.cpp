#include "system/WorldGenSystem.hpp"
#include "World.hpp"
#include "script/lua/LuaHelpers.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <cstring>
#include <doctest/doctest.h>
#include <filesystem>

void InitScriptVMsForTests();

TEST_CASE("WorldGenSystem deterministically generates rooms")
{
    InitScriptVMsForTests();

    const int x = 432;
    const int y = 123;
    const std::filesystem::path room_db = "./db/rooms/432_123";

    std::filesystem::remove(room_db);
    Room::mapRooms.clear();

    World world;
    world.systems.push_back(std::make_unique<WorldGenSystem>(world, 99));

    auto room = Room::get(world, x, y);
    REQUIRE(room != nullptr);
    CHECK(!room->name.empty());

    std::vector<std::string> first_generation;
    for (const auto& thing : room->things)
        first_generation.push_back(thing->name);

    Room::mapRooms.clear();

    auto same_room = Room::get(world, x, y);
    REQUIRE(same_room != nullptr);
    CHECK(same_room->name == room->name);

    std::vector<std::string> second_generation;
    for (const auto& thing : same_room->things)
        second_generation.push_back(thing->name);

    CHECK(second_generation == first_generation);
}
