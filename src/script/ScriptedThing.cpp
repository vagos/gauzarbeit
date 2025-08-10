#include "script/ScriptedThing.hpp"

#include "Helpers.hpp"
#include "Quest.hpp"
#include "Room.hpp"
#include "Server.hpp"
#include "script/LuaHelpers.hpp"
#include "script/ScriptedAchiever.hpp"
#include "script/ScriptedAttackable.hpp"
#include "script/ScriptedInspectable.hpp"
#include "script/ScriptedNetworked.hpp"
#include "script/ScriptedNotifier.hpp"
#include "script/ScriptedPhysical.hpp"
#include "script/ScriptedTalker.hpp"
#include "script/ScriptedTasker.hpp"
#include "script/ScriptedUsable.hpp"

#include <memory>
#include <string>

ScriptedThing::ScriptedThing(const std::string& name, const std::string& script_dir) : Thing(name)
{
    // Create components
    _usable = std::make_unique<ScriptedUsable>();
    _attackable = std::make_unique<ScriptedAttackable>();
    _notifier = std::make_unique<ScriptedNotifier>();
    _tasker = std::make_unique<ScriptedTasker>();
    _physical = std::make_unique<ScriptedPhysical>();
    _inspectable = std::make_unique<ScriptedInspectable>();
    _talker = std::make_unique<ScriptedTalker>();
    _achiever = std::make_unique<Achiever>();
    _networked = std::make_unique<ScriptedNetworked>();

    lua_getglobal(L, name.c_str());

    if (lua_isnil(L, -1))
    {
        lua_newtable(L);
        lua_setglobal(L, name.c_str()); // Create a Lua table.
    }

    std::string filename(script_dir + name + ".lua");

    lua_newtable(L);

    luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_pushlightuserdata(L, this);

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    lua_setmetatable(L, -2);

    CheckLua(L, luaL_dofile(L, filename.c_str()));

    // doInit
    lua_getglobal(L, name.c_str());
    lua_getfield(L, -1, "doInit");

    if (lua_isfunction(L, -1))
    {
        lua_pushlightuserdata(L, this);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
    }
}

int ScriptedThing::Index(lua_State* L)
{
    ScriptedThing* ptrThing = (ScriptedThing*)lua_touserdata(L, 1);
    const char* index = lua_tostring(L, 2);

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    lua_getfield(L, -1, index);

    if (lua_isnil(L, -1))
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ptrThing->luaRef);
        lua_getfield(L, -1, index);

        if (lua_isnil(L, -1))
        {
            lua_getglobal(L, ptrThing->name.c_str());
            lua_getfield(L, -1, index);

            if (lua_isnil(L, -1))
                return 0;
        }
    }

    return 1;
}

int ScriptedThing::NewIndex(lua_State* L)
{
    ScriptedThing* ptrThing = (ScriptedThing*)lua_touserdata(L, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, ptrThing->luaRef);

    const char* index = lua_tostring(L, 2);

    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, 4);

    return 0;
}

int ScriptedThing::GetName(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    auto t = ptrThing->physical()->current_room->getAnything(ptrThing->name);

    assert(t);

    lua_pushstring(L, ptrThing->inspectable()->getName(t).c_str());

    return 1;
}

int ScriptedThing::SendMessage(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Log(ptrThing->name);

    std::size_t s_l;
    const char* s = lua_tolstring(L, 2, &s_l);

    std::string message{s, s_l};

    ptrThing->networked()->addResponse(message);

    return 0;
}

int ScriptedThing::LoseItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    ptrThing->physical()->loseItem(item);

    return 0;
}

int ScriptedThing::DropItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    ptrThing->physical()->dropItem(item);

    return 0;
}

int ScriptedThing::GetThing(lua_State* L) // Return a thing from inside the room.
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isstring(L, 2));
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    auto thing = ptrThing->physical()->current_room->getThing(lua_tostring(L, 2));

    if (!thing)
        return 0;

    lua_pushlightuserdata(L, thing.get());

    return 1;
}

int ScriptedThing::GainItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (lua_isstring(L, 2))
    {
        auto t_n = std::string(lua_tostring(L, 2));
        auto item = std::make_shared<ScriptedThing>(t_n);

        ptrThing->physical()->gainItem(item);
    }

    else if (lua_isuserdata(L, 2))
    {
        Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);
        auto item = GetSmartPtr(ptrThing->physical()->current_room->things, ptrThingItem);

        if (!item)
            return 0;

        ptrThing->physical()->current_room->removeThing(item);
        ptrThing->physical()->gainItem(item);
    }

    return 0;
}

int ScriptedThing::HasItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    lua_pushboolean(L, item && ptrThing->physical()->hasItem(item));

    return 1;
}

int ScriptedThing::BroadcastMessage(lua_State* L)
{

    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    assert(lua_isstring(L, 2));
    const std::string message{lua_tostring(L, 2)};

    auto p = GetSmartPtr(ptrThing->physical()->getRoom()->players, ptrThing);
    assert(p != nullptr);

    p->notifier()->setEventPayload(message);
    p->notifier()->doNotify(p, Event::Type::Message);

    return 0;
}

int ScriptedThing::AddTask(lua_State* L)
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isstring(L, 2));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string task_description(lua_tostring(L, 2));

    if (ptrThing->_tasker)
    {
        lua_pushnumber(L, ptrThing->tasker()->addTask(task_description));
        return 1;
    }

    return 0;
}

int ScriptedThing::TickTask(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_tasker)
    {
        ptrThing->tasker()->tickTask((int)lua_tonumber(L, 2));
    }

    return 0;
}

int ScriptedThing::GainXP(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_achiever)
    {
        ptrThing->achiever()->gainXP(lua_tonumber(L, 2));
    }

    return 0;
}

int ScriptedThing::GetLevel(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_achiever)
    {
        lua_pushnumber(L, ptrThing->achiever()->getLevel());

        return 1;
    }

    return 0;
}

int ScriptedThing::GetEventInfo(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_notifier)
    {
        lua_newtable(L);

        lua_pushstring(L, ptrThing->notifier()->event.verb.c_str());
        lua_setfield(L, -2, "verb");

        lua_pushstring(L, ptrThing->notifier()->event.target.c_str());
        lua_setfield(L, -2, "target");

        lua_pushstring(L, ptrThing->notifier()->event.object.c_str());
        lua_setfield(L, -2, "object");

        lua_pushstring(L, ptrThing->notifier()->event.extra.c_str());
        lua_setfield(L, -2, "extra");

        return 1;
    }

    return 0;
}

int ScriptedThing::EquipItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    if (item)
        ptrThing->physical()->equipItem(item);

    return 0;
}

// Returns a Player with the given name in the room the thing is in
int ScriptedThing::GetPlayer(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (!lua_isstring(L, 2))
        return 0;

    std::string t_n(lua_tostring(L, 2));

    Log("NAME: " << t_n);

    auto t = ptrThing->physical()->current_room->getPlayer(t_n);

    if (!t)
        return 0;

    lua_pushlightuserdata(L, t.get());

    return 1;
}

int ScriptedThing::GainQuest(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    if (!lua_isstring(L, 2))
        return 0;

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string q_n(lua_tostring(L, 2));

    ptrThing->achiever()->gainQuest(std::make_shared<ScriptedQuest>(q_n));

    return 0;
}

int ScriptedThing::DoSay(lua_State* L)
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isuserdata(L, 2));

    if (!lua_isstring(L, 3))
        return 0;

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingTarget = (Thing*)lua_touserdata(L, 2);

    std::string s(lua_tostring(L, 3));

    std::stringstream msg;

    msg << ptrThing->name << ": " << s;

    ptrThingTarget->networked()->addResponse(msg.str());

    return 0;
}

int ScriptedThing::GetHP(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (!ptrThing->_attackable)
        return 0;

    lua_pushnumber(L, (float)ptrThing->attackable()->current_health);

    return 1;
}

int ScriptedThing::DoAttack(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    Thing* ptrThingTarget = (Thing*)lua_touserdata(L, 2);

    if (!(ptrThing->_attackable && ptrThingTarget->_attackable))
        return 0;

    auto t = GetSmartPtr(ptrThing->physical()->current_room->things, ptrThing);
    auto t_t = ptrThing->physical()->current_room->getAnything(ptrThingTarget->name);

    if (!t)
        return 0;

    ptrThing->attackable()->doAttack(t, t_t);

    return 0;
}

int GetStat(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    std::string s_n(lua_tostring(L, 2));

    float val = ptrThing->achiever()->getStat(s_n);

    lua_pushnumber(L, val);

    return 1;
}

int SetStat(lua_State* L)
{
    assert(lua_isnumber(L, 3));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string s_n(lua_tostring(L, 2));
    float val = (float)lua_tonumber(L, 3);

    ptrThing->achiever()->setStat(s_n, val);

    return 0;
}

int Gauzarbeit_Spawn(lua_State* L)
{
    if (lua_isuserdata(L, 1))
    {
        Room* r = (Room*)lua_touserdata(L, 1);
        std::string t_n(lua_tostring(L, 2));
        auto t = std::make_shared<ScriptedThing>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, r->x, r->y);
        else
            r->addThing(t);

        return 0;
    }
    else
    {
        int x = (int)lua_tonumber(L, 1);
        int y = (int)lua_tonumber(L, 2);

        std::string t_n(lua_tostring(L, 3));

        auto r = Room::get(x, y);

        assert(r);

        auto t = std::make_shared<ScriptedThing>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, r->x, r->y);
        else
            r->addThing(t);
    }

    return 0;
}

int Gauzarbeit_Room(lua_State* L)
{
    std::string r_t(lua_tostring(L, 1));

    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);

    auto r = Room::get(x, y);

    r->name = r_t;
    r->doGeneration();

    lua_pushlightuserdata(L, r.get());

    return 1;
}

int Gauzarbeit_ColorString(lua_State* L)
{
    assert(lua_isstring(L, 1));
    assert(lua_isnumber(L, 2));

    std::string s(lua_tostring(L, 1));

    auto c = (Color)lua_tonumber(L, 2);

    lua_pushstring(L, ColorString(s, c).c_str());

    return 1;
}

int Gauzarbeit_LoadDB(lua_State* L)
{
    std::string db_line;

    Networked::getDB() >> db_line;

    lua_pushstring(L, db_line.c_str());

    return 1;
}

void ScriptedThing::InitLua()
{
    luaL_openlibs(L);

    // Create the Gauzarbeit.Thing metatable
    luaL_newmetatable(L, "Gauzarbeit.Thing");

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    const luaL_Reg thingMethods[] = {{"__index", ScriptedThing::Index},
                                     {"__newindex", ScriptedThing::NewIndex},
                                     {"getName", ScriptedThing::GetName},
                                     {"getType", ScriptedThing::GetName},
                                     {"setStat", SetStat},
                                     {"getStat", GetStat},
                                     {"sendMessage", ScriptedThing::SendMessage},
                                     {"doSay", ScriptedThing::DoSay},
                                     {"loseItem", ScriptedThing::LoseItem},
                                     {"dropItem", ScriptedThing::DropItem},
                                     {"equipItem", ScriptedThing::EquipItem},
                                     {"getThing", ScriptedThing::GetThing},
                                     {"getPlayer", ScriptedThing::GetPlayer},
                                     {"gainItem", ScriptedThing::GainItem},
                                     {"hasItem", ScriptedThing::HasItem},
                                     {"broadcastMessage", ScriptedThing::BroadcastMessage},
                                     {"addTask", ScriptedThing::AddTask},
                                     {"tickTask", ScriptedThing::TickTask},
                                     {"gainXP", ScriptedThing::GainXP},
                                     {"getEventInfo", ScriptedThing::GetEventInfo},
                                     {"getLevel", ScriptedThing::GetLevel},
                                     {"gainQuest", ScriptedThing::GainQuest},
                                     {"doAttack", ScriptedThing::DoAttack},
                                     {NULL, NULL}};

    luaL_setfuncs(L, thingMethods, 0);

    // Create the global Gauzarbeit table.
    lua_newtable(L);

    // Set the Event constants

    {
        // Create the Gauzarbeit.Event table
        lua_newtable(L);

        lua_pushnumber(L, (int)Event::Type::Enter);
        lua_setfield(L, -2, "Enter");

        lua_pushnumber(L, (int)Event::Type::Ask);
        lua_setfield(L, -2, "Ask");

        lua_pushnumber(L, (int)Event::Type::Do);
        lua_setfield(L, -2, "Do");

        lua_pushnumber(L, (int)Event::Type::Move);
        lua_setfield(L, -2, "Move");

        lua_pushnumber(L, (int)Event::Type::Inspect);
        lua_setfield(L, -2, "Inspect");

        lua_pushnumber(L, (int)Event::Type::Kill);
        lua_setfield(L, -2, "Kill");

        lua_pushnumber(L, (int)Event::Type::Gain);
        lua_setfield(L, -2, "Gain");

        lua_setfield(L, -2, "Event");
    }

    // Create Gauzarbeit.Color table
    {
        lua_newtable(L);

        lua_pushnumber(L, (int)Color::Red);
        lua_setfield(L, -2, "Red");

        lua_pushnumber(L, (int)Color::Green);
        lua_setfield(L, -2, "Green");

        lua_pushnumber(L, (int)Color::Blue);
        lua_setfield(L, -2, "Blue");

        lua_setfield(L, -2, "Color");
    }

    const luaL_Reg gauzarbeitFuncs[] = {{"Spawn", Gauzarbeit_Spawn},
                                        {"GetRoom", Gauzarbeit_Room},
                                        {"ColorString", Gauzarbeit_ColorString},
                                        {"GetDBLine", Gauzarbeit_LoadDB},
                                        // CreateStat
                                        {NULL, NULL}};

    luaL_setfuncs(L, gauzarbeitFuncs, 0);

    lua_setglobal(L, "Gauzarbeit");

    CheckLua(L, luaL_dofile(L, "scripts/Init.lua"));

    // Load MOTD

    auto& L = ScriptedThing::L;

    lua_getglobal(L, "MOTD");

    Server::MOTD.assign(lua_tostring(L, -1));
}

lua_State* ScriptedThing::L = luaL_newstate();
