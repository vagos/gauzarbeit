#include "script/LuaHelpers.hpp"
#include "Helpers.hpp"

bool CheckLua(lua_State* L, int r)
{
    if (r != LUA_OK)
    {
        std::string errormsg = lua_tostring(L, -1);
        Log(errormsg);
        throw std::runtime_error("Lua error: " + errormsg);
    }
    return true;
}
