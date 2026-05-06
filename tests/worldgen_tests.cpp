#include "system/WorldGenSystem.hpp"
#include "World.hpp"
#include "script/js/ScriptedThing.hpp"
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
    world.systems.push_back(std::make_unique<WorldGenSystem>(99));

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

TEST_CASE("Scripting can call GenerateRoom")
{
    InitScriptVMsForTests();

    Room::mapRooms.clear();

    World world;
    world.systems.push_back(std::make_unique<WorldGenSystem>(7));

    lua_State* L = ScriptedThing_Lua::L;
    lua_settop(L, 0);
    CheckLua(L, luaL_dostring(L,
                              "local room = Gauzarbeit.GenerateRoom(21, 22)\n"
                              "return room ~= nil"));
    CHECK(lua_toboolean(L, -1) == 1);
    lua_settop(L, 0);
    CHECK(Room::mapRooms[(21 & 0xFFFF) << 16 | (22 & 0xFFFF)] != nullptr);

    constexpr const char* js_typeof_generate_room = "typeof Gauzarbeit.GenerateRoom";
    JSValue typeof_val =
        JS_Eval(ScriptedThing_JS::ctx, js_typeof_generate_room,
                std::strlen(js_typeof_generate_room), "<typeof>", JS_EVAL_TYPE_GLOBAL);
    const char* typeof_str = JS_ToCString(ScriptedThing_JS::ctx, typeof_val);
    CHECK(std::string(typeof_str ? typeof_str : "") == "function");
    JS_FreeCString(ScriptedThing_JS::ctx, typeof_str);
    JS_FreeValue(ScriptedThing_JS::ctx, typeof_val);

    constexpr const char* js_test =
        "const room = Gauzarbeit.GenerateRoom(23, 24); room !== undefined;";
    JSValue val = JS_Eval(ScriptedThing_JS::ctx, js_test, std::strlen(js_test), "<test>",
                          JS_EVAL_TYPE_GLOBAL);
    std::string js_error;
    if (JS_IsException(val))
    {
        JSValue exception = JS_GetException(ScriptedThing_JS::ctx);
        const char* error = JS_ToCString(ScriptedThing_JS::ctx, exception);
        js_error = error ? error : "<js exception>";
        JS_FreeCString(ScriptedThing_JS::ctx, error);
        JS_FreeValue(ScriptedThing_JS::ctx, exception);
    }
    CHECK_MESSAGE(!JS_IsException(val), js_error);
    CHECK(JS_ToBool(ScriptedThing_JS::ctx, val) == 1);
    JS_FreeValue(ScriptedThing_JS::ctx, val);
}
