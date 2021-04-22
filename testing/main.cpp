#include <bits/c++config.h>

extern "C"
{
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/lualib.h>
}

#include <string>
#include <iostream>
#include <filesystem>

struct Thing
{
    int a = 7;

    void set(int x) { a = x;}

    void print() {std::cout << "My value: " << a << "\n";}
    
    void doUpdate(Thing * owner)
    {
        
    }

    int doUpdateLua(lua_State * L)
    {

        lua_getglobal(L, "doUpdate");
        
        lua_pushlightuserdata(L, (void *)this);

        luaL_getmetatable(L, "Gauzarbeit.Thing");
        lua_setmetatable(L, -2);
    

        lua_pcall(L, 1, 0, 0);
        
        return 0;
    }


};


int lua_HostFunction(lua_State * L)
{
    
    std::cout << "[C++] Here!";

    return 0;
}

int lua_NewThing(lua_State * L)
{
    Thing * lua_Thing = new Thing();
   
    lua_pushlightuserdata(L, (void *)lua_Thing);

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    lua_setmetatable(L, -2);

    return 1;

}

int lua_PrintSelf(lua_State * L)
{
    
    Thing * thing_ptr = (Thing *)luaL_checkudata(L, 1, "Gauzarbeit.Thing");
    
    thing_ptr -> print();

    return 0;
}

int lua_SetThing(lua_State * L)
{
    void * thing_ptr = luaL_checkudata(L, 1, "Gauzarbeit.Thing");
    //void * thing_ptr = lua_touserdata(L, 1);

    luaL_argcheck(L, thing_ptr != nullptr, 1, "Expected Thing!");
    
    Thing * t = (Thing *)thing_ptr; 

    int value = luaL_checkinteger(L, 2); 

    t -> a = value;

    return 0;

}


    
int main()
{
    
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    luaL_newmetatable(L, "Gauzarbeit.Thing");

    //lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    const luaL_Reg myFuncs[] = {
    {"NewThing", lua_NewThing},
    {NULL, NULL}
    };

    const luaL_Reg myMethods[] = {
    {"SetValue", lua_SetThing},
    {"CreateFriend", lua_NewThing},
    {"PrintSelf", lua_PrintSelf},
    {NULL, NULL}
    };
    
    luaL_setfuncs(L, myMethods, 0);

    lua_newtable(L);
    luaL_setfuncs(L, myFuncs, 0);
    lua_setglobal(L, "Gauzarbeit");

    
    //lua_register(L, "NewThing", lua_NewThing);
    //lua_register(L, "SetThing", lua_SetThing);
    
    int r = luaL_dofile( L, "test.lua" );
    
    if ( r == LUA_OK)
    {
        //lua_getglobal(L, "DoAThing");

    }
    else 
    {
        std::string errorMsg = lua_tostring(L, -1); 
        std::clog << errorMsg;
    }

    Thing * myThing = new Thing();

    myThing -> doUpdateLua(L);

    lua_close(L);

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("./"))
        std::cout << dirEntry << std::endl;
    return 0;
}
