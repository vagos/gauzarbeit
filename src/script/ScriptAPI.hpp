#pragma once

#include <quickjs.h>

struct lua_State;

struct ScriptConstant
{
    const char* name;
    int value;
};

struct ScriptAPI
{
    static constexpr const char* kThingMethods[] = {
        "getName",  "getType",          "setStat",   "getStat",  "sendMessage", "doSay",
        "loseItem", "dropItem",         "equipItem", "getThing", "getPlayer",   "gainItem",
        "hasItem",  "broadcastMessage", "addTask",   "tickTask", "gainXP",      "getEventInfo",
        "getLevel", "gainQuest",        "doAttack",  nullptr,
    };

    static constexpr const char* kGauzarbeitFuncs[] = {
        "Spawn", "GetRoom", "GenerateRoom", "ColorString", "GetDBLine", nullptr,
    };

    static constexpr ScriptConstant kEventConstants[] = {
        {"Invalid", 0},     {"Message", 1},   {"Say", 2},      {"Chat", 3},    {"Move", 4},
        {"Enter", 5},       {"Use", 6},       {"Do", 7},       {"Buy", 8},     {"Info", 9},
        {"Ask", 10},        {"Help", 11},     {"Inspect", 12}, {"Attack", 13}, {"Attacked", 14},
        {"Kill", 15},       {"Death", 16},    {"Leave", 17},   {"Gain", 18},   {"Look", 19},
        {"Gain_Quest", 20}, {"Register", 21}, {"Custom", 22},  {nullptr, 0},
    };

    static constexpr ScriptConstant kColorConstants[] = {
        {"None", 0},   {"White", 1}, {"Red", 2},     {"Green", 3}, {"Blue", 4},
        {"Yellow", 5}, {"Cyan", 6},  {"Magenta", 7}, {nullptr, 0},
    };
};

void VerifyLuaAPI(lua_State* L);
void VerifyJSAPI(JSContext* ctx, JSValue proto);
