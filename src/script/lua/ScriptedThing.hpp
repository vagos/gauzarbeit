#ifndef SCRIPTED_THING_LUA_HPP
#define SCRIPTED_THING_LUA_HPP

#include "script/ScriptedThing.hpp"
#include <unordered_map>

class lua_State;

class ScriptedThing_Lua : public script::ScriptedThing
{

  public:
    ScriptedThing_Lua(const std::string& name, const std::string& script_dir = "./ext/things/");
    ~ScriptedThing_Lua();

    static int Index(lua_State* L);

    static int NewIndex(lua_State* L);

    static int IsValid(lua_State* L);

    static int GetName(lua_State* L);

    static int LoseItem(lua_State* L);
    static int GainItem(lua_State* L);
    static int DropItem(lua_State* L);
    static int Destroy(lua_State* L);
    static int GetThing(lua_State* L);
    static int GetRoom(lua_State* L);
    static int GetThings(lua_State* L);
    static int GetPlayers(lua_State* L);
    static int HasItem(lua_State* L);
    static int MoveTo(lua_State* L);
    static std::string Eval(const std::string& code, Thing* admin = nullptr);

    static int GetStat(lua_State* L);
    static int SetStat(lua_State* L);

    static int GetPlayer(lua_State* L);

    static int GainXP(lua_State* L);
    static int GetLevel(lua_State* L);
    static int GiveTask(lua_State* L);

    static int AddTask(lua_State* L);
    static int TickTask(lua_State* L);
    static int HasTask(lua_State* L);
    static int HasDoneTask(lua_State* L);
    static int RewardTask(lua_State* L);

    static int SendMessage(lua_State* L);
    static int DoSay(lua_State* L);
    static int BroadcastMessage(lua_State* L);

    static int DoAttack(lua_State* L);
    static int DoLater(lua_State* L);

    static void Init();
    static void RunScheduledCallbacks(double current_time);

    static int GetLuaRef(const Thing* thing);

    static lua_State* L;

    static std::unordered_map<const Thing*, int> luaRefs;
    int luaRef;
};

#endif
