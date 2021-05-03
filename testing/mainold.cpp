#include "lua.hpp"
#include <lua.h>

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
        
        CheckLua(L, luaL_dofile(L, filename.c_str()) );        

    }

    static Thing * doInit(const std::string& name, lua_State * L)
    {
        Thing * ptrThing = (Thing * )lua_newuserdata(L, sizeof(Thing));

        new (ptrThing) Thing(name, L);

        lua_pushlightuserdata(L, ptrThing);

        lua_newtable(L);
        lua_setuservalue(L, -2);
        
        luaL_getmetatable(L, "Gauzarbeit.Thing");
        lua_setmetatable(L, -3);

        lua_pushvalue(L, 1); // userdata is a table???

        ptrThing -> luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
        
        ptrThing -> doInit(ptrThing, L);

        return (Thing *)ptrThing;
    }

   
    void doUpdate(Thing * owner, lua_State * L)
    {
        lua_getglobal(L, name.c_str());

        lua_getfield(L, -1, "doUpdate");

        lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
        
    }

    void doInit(Thing * owner, lua_State * L)
    {
        lua_getglobal(L, name.c_str());

        lua_getfield(L, -1, "doInit");

        lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
        
    }

    static int ThingIndex(lua_State * L)
    {
        assert(lua_isuserdata(L, 1)); // 1
        assert(lua_isstring(L, 2));   // 2
        
        Thing * thing = (Thing *)lua_touserdata(L, 1);
        const char * index = lua_tostring(L, 2);

        std::clog << "Index: " << index << " " << thing << "\n";
    
        lua_getuservalue(L, 1);

        lua_getfield(L, 2, index);
    
        if ( lua_isnil(L, -1) )
        {
            lua_getglobal(L, thing -> name.c_str());
            lua_pushstring(L, index);
            lua_rawget(L, -2);
        }

        return 1;
    }

    static int ThingNewIndex(lua_State * L)
    {
        Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

        lua_getuservalue(L, 1);

        assert(lua_istable(L, 1));
        
        const char * index = lua_tostring(L, 2);

        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_settable(L, -4);	


        return 0;
    
    }

    static int getName(lua_State * L)
    {
        assert(lua_istable(L, 1));
        Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

        lua_pushstring(L, ptrThing -> name.c_str() );

        return 1;

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
   // {"__newindex", Thing::ThingNewIndex},
    {"getName", Thing::getName},
    {NULL, NULL}
    };


    luaL_setfuncs(L, thingMethods, 0);

    Thing * myThing = Thing::doInit("Chair", L); 
    myThing -> doUpdate(myThing, L);
    //Thing * myThing2 = Thing::doInit("Chair", L); 

    //myThing -> doUpdate(myThing, L);
    //myThing -> doUpdate(myThing, L);
    //myThing2 -> doUpdate(myThing, L);

    //luaL_newetatable(L, "Gauzarbeit.Thing");

    //lua_pushstring(L, "__index");
    //lua_pushvalue(L, -2);
    //lua_settable(L, -3);

    //const luaL_Reg myFuncs[] = {
    //{"NewThing", lua_NewThing},
    //{NULL, NULL}
    //};

    //const luaL_Reg myMethods[] = {
    //{"SetValue", lua_SetThing},
    //{"GetValue", lua_GetValue},
    //{"CreateFriend", lua_NewThing},
    //{"PrintSelf", lua_PrintSelf},
    //{NULL, NULL}
    //};
    //
    //luaL_setfuncs(L, myMethods, 0);

    //lua_newtable(L);
    //luaL_setfuncs(L, myFuncs, 0);
    //lua_setglobal(L, "Gauzarbeit");

    //Thing * myThing = new Thing("Thing1");
    //Thing * myThing2 = new Thing("Thing2");

    //auto start = std::chrono::high_resolution_clock::now();

//      lua_newtable(L);
//      lua_setglobal(L, "Chair");
//  
//      if (luaL_dofile(L, "Thing1.lua") != LUA_OK)
//      {
//          std::string errormsg = lua_tostring(L, -1);
//  		std::cout << errormsg << std::endl;
//      }
//      
//      lua_getglobal(L, "Chair");
//      
//      lua_getfield(L, -1, "doInit"); 
//      lua_pushvalue(L, -2);
//      CheckLua(L, lua_pcall(L, 1, 0, 0));
//  
//      Thing * myThing = new Thing("Vagoz");
//  
//      lua_pushstring(L, "m");
//      lua_pushlightuserdata(L, myThing); 
//  //    lua_pushnumber(L, 69);
//      lua_settable(L, -3);
//  
//      //lua_getfield(L, -1, "Thing");
//      //Thing * t = (Thing *)lua_touserdata(L, 1);
//  
//      //t->print();
//  
//      lua_pushstring(L, "m");
//      lua_gettable(L, -2);
//                                   
//      //int n = lua_tonumber(L, -1);    
//      void * n = lua_touserdata(L, -1);
//      std::clog << "N: " << n << "\n";
//  
//      lua_pop(L , 1);
//      
//      static_cast<Thing *>(n) -> print();

    lua_close(L);

//    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("./"))
//        std::cout << dirEntry << std::endl;
    return 0;
}


// int lua_SetThing(lua_State * L)
// {
//     void * thing_ptr = luaL_checkudata(L, 1, "Gauzarbeit.Thing");
// 
//     luaL_argcheck(L, thing_ptr != nullptr, 1, "Expected Thing!");
//     
//     Thing * t = (Thing *)thing_ptr; 
// 
//     int value = luaL_checkinteger(L, 2); 
// 
//     t -> a = value;
// 
//     return 0;
// 
// }

