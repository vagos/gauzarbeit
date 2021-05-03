#include "lua.hpp"

extern "C"
{
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/lualib.h>
}

#include <string>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <cassert>
#include <cstring>

bool CheckLua(lua_State *L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << "[C++] " << errormsg << std::endl;
		return false;
	}
	return true;
}

struct Thing
{
    std::string name;

    int luaRef;


    Thing (const std::string name_, lua_State * L): name(name_)
    {
        std::string filename = name + ".lua";

        lua_newtable(L);
        lua_setglobal(L, name.c_str());
        lua_pop(L, 1);

        lua_newtable(L);

        luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

        lua_pushlightuserdata(L, this);

        luaL_getmetatable(L, "Gauzarbeit.Thing");
        lua_setmetatable(L, -2);
        
        CheckLua(L, luaL_dofile(L, filename.c_str()) );        

    }

    void doUpdate(Thing * owner, lua_State * L)
    {
        lua_getglobal(L, name.c_str());

        lua_getfield(L, -1, "doUpdate");

        lua_pushlightuserdata(L, this);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
        
    }

    void doInit(Thing * owner, lua_State * L)
    {
        lua_getglobal(L, name.c_str());

        lua_getfield(L, -1, "doInit");

        lua_pushlightuserdata(L, this);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
        
    }

    static int ThingIndex(lua_State * L)
    {
        assert(lua_isuserdata(L, 1)); // 1
        assert(lua_isstring(L, 2));   // 2
        
        Thing * thing = (Thing *)lua_touserdata(L, 1);
        const char * index = lua_tostring(L, 2);

        lua_rawgeti(L, LUA_REGISTRYINDEX, thing -> luaRef);

        lua_getfield(L, 3, index);

        if (lua_isnil(L, -1))
        {
            luaL_getmetatable(L, "Gauzarbeit.Thing");
            lua_getfield(L, -1, index);
        }
    
        return 1;
    }

    static int ThingNewIndex(lua_State * L)
    {
        Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

        lua_rawgeti(L, LUA_REGISTRYINDEX, ptrThing -> luaRef);

        assert(lua_istable(L, -1));
        
        const char * index = lua_tostring(L, 2);
        
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_settable(L, 4);	

        return 0;
    
    }

    static int getName(lua_State * L)
    {
        Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

        lua_pushstring(L, ptrThing -> name.c_str() );

        return 1;

    }

    static int printRef(lua_State * L)
    {
        Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

        std::cout << "My ref is " << ptrThing -> luaRef << "\n";

        return 0;
    }

};





int main()
{
    
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    luaL_newmetatable(L, "Gauzarbeit.Thing");     

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    const luaL_Reg thingMethods[] = {
    {"__index", Thing::ThingIndex},
    {"__newindex", Thing::ThingNewIndex},
    {"getName", Thing::getName},
    {"printRef", Thing::printRef},
    {NULL, NULL}
    };

    luaL_setfuncs(L, thingMethods, 0);

    Thing * myThing = new Thing("Chair", L); 
    myThing -> doInit(myThing, L);
    myThing -> doUpdate(myThing, L);
    Thing * myThing2 = new Thing("Chair", L); 
    myThing2 -> doInit(myThing, L);
    
    myThing -> doUpdate(myThing, L);
    myThing2 -> doUpdate(myThing, L);

    lua_close(L);

    return 0;
}
