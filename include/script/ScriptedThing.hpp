#ifndef SCRIPTED_THING_HPP
#define SCRIPTED_THING_HPP

#include "thing/Thing.hpp"

class lua_State;

class ScriptedThing : public Thing
{

  public:
	ScriptedThing(const std::string& name, const std::string& script_dir = "./scripts/things/");

	static int Index(lua_State* L);

	static int NewIndex(lua_State* L);

	static int IsValid(lua_State* L);

	static int GetName(lua_State* L);

	static int GetEventInfo(lua_State* L);

	static int LoseItem(lua_State* L);
	static int GainItem(lua_State* L);
	static int DropItem(lua_State* L);
	static int GetThing(lua_State* L);
	static int HasItem(lua_State* L);
	static int EquipItem(lua_State* L);

	static int GetPlayer(lua_State* L);

	static int GainXP(lua_State* L);
	static int GetLevel(lua_State* L);
	static int GainQuest(lua_State* L);

	static int AddTask(lua_State* L);
	static int TickTask(lua_State* L);

	static int SendMessage(lua_State* L);
	static int DoSay(lua_State* L);
	static int BroadcastMessage(lua_State* L);

	static int SetMaxHealth(lua_State* L);
	static int SetAttack(lua_State* L);
	static int SetDefense(lua_State* L);
	static int GetHP(lua_State* L);
	static int DoAttack(lua_State* L);

	static void InitLua();

	static lua_State* L;

	int luaRef;
};

#endif
