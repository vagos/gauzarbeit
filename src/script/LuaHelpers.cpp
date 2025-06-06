#include "script/LuaHelpers.hpp"

bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		throw std::runtime_error("Lua error: " + errormsg);
	}
	return true;
}
