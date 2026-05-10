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
        "getName",   "getType",    "setStat",   "getStat",  "sendMessage",  "doSay",
        "loseItem",  "dropItem",   "equipItem", "getThing", "getPlayer",    "getRoom",
        "getThings", "getPlayers", "gainItem",  "hasItem",  "doMove",       "broadcastMessage",
        "addTask",   "tickTask",   "giveTask",  "gainXP",   "getEventInfo", "getLevel",
        "doAttack",  nullptr,
    };

    static constexpr const char* kGauzarbeitFuncs[] = {
        "Spawn", "GetRoom", "SetRoom", "ColorString", "GetDBLine", "WithChance", nullptr,
        // TODO 
        /*
        SetRoom -> CreateRoom
        Delete
        */
    };

    static constexpr ScriptConstant kEventConstants[] = {
        {"Message", 0},   {"Say", 1},     {"Chat", 2},     {"Move", 3},    {"Enter", 4},
        {"Use", 5},       {"Do", 6},      {"Buy", 7},      {"Info", 8},    {"Ask", 9},
        {"Help", 10},     {"Inspect", 11}, {"Attack", 12}, {"Attacked", 13}, {"Kill", 14},
        {"Death", 15},    {"Leave", 16},  {"Gain", 17},    {"Provide", 18}, {"Register", 19},
        {"Custom", 20},   {"Invalid", 21}, {nullptr, 0},
    };

    static constexpr ScriptConstant kColorConstants[] = {
        {"None", 0},   {"White", 1}, {"Red", 2},     {"Green", 3}, {"Blue", 4},
        {"Yellow", 5}, {"Cyan", 6},  {"Magenta", 7}, {nullptr, 0},
    };

    static constexpr ScriptConstant kDirectionConstants[] = {
        {"North", 0}, {"South", 1}, {"East", 2}, {"West", 3}, {nullptr, 0},
    };
};

void VerifyLuaAPI(lua_State* L);
void VerifyJSAPI(JSContext* ctx, JSValue proto);
