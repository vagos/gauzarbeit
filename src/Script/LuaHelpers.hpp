#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP

#include <string>
#include <iostream>
#include <memory>
#include <algorithm>

class Thing;

#include "lua.hpp"

extern "C"
{
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/lualib.h>
}

bool CheckLua(lua_State *L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}

template<typename T>
const std::shared_ptr<Thing> GetSmartPtr(const T& c, Thing * t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(), [&t_ptr](const std::shared_ptr<Thing> &t)
            {return t.get() == t_ptr;});    
    
    return r != c.end() ? *r : nullptr;
}

#endif
