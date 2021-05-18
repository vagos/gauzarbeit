#include "LuaHelpers.hpp"
#include "ScriptedThing.hpp"
#include "ScriptedUsable.hpp"
#include "ScriptedAttackable.hpp"
#include "../Room.hpp"
#include <lua.h>
#include <memory>


ScriptedThing::ScriptedThing(const std::string& name):
    Thing(name) 
{
    usable = std::make_unique<ScriptedUsable>(); // Create components
    attackable = std::make_unique<ScriptedAttackable>();

        lua_getglobal(L, sName.c_str());
        
        if (lua_isnil(L, -1))
        {
            lua_newtable(L);
            lua_setglobal(L, sName.c_str()); // Create a Lua table.
        }

        std::string filename( scriptDir + sName + ".lua" );

        lua_newtable(L);

        luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

        lua_pushlightuserdata(L, this);

        luaL_getmetatable(L, "Gauzarbeit.Thing");
        lua_setmetatable(L, -2);
        
        CheckLua(L, luaL_dofile(L, filename.c_str() ));

        // doInit
        
        lua_getglobal(L, sName.c_str());
        lua_getfield(L, -1, "doInit");

        if (lua_isfunction(L, -1))
        {
           lua_pushlightuserdata(L, this); 

           CheckLua(L, lua_pcall(L, 1, 0, 0));
        }

}

int ScriptedThing::Index(lua_State *L)
{
        ScriptedThing * ptrThing = (ScriptedThing *)lua_touserdata(L, 1);
        const char * index = lua_tostring(L, 2);
        
        luaL_getmetatable(L, "Gauzarbeit.Thing");
        
        lua_getfield(L, -1, index);
        
        if (lua_isnil(L, -1))
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ptrThing -> luaRef); 
            lua_getfield(L, -1, index);
        }

        return 1;
}

int ScriptedThing::NewIndex(lua_State *L)
{
    ScriptedThing * ptrThing = (ScriptedThing *)lua_touserdata(L, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, ptrThing -> luaRef);

    const char * index = lua_tostring(L, 2);
    
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, 4);	

    return 0;

}

int ScriptedThing::GetName(lua_State *L)
{
    Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

    lua_pushstring(L, ptrThing -> sName.c_str());

    return 1;
}

int ScriptedThing::SendMessage(lua_State * L)
{
    Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

    std::string message{ lua_tostring(L, 2) };
    
    ptrThing -> networked -> addResponse(message);

    return 0;

}

int ScriptedThing::SetMaxHealth(lua_State *L)
{
    assert(lua_isuserdata(L, 1));

    Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

    int max_health = (int)lua_tonumber(L, 2);

    ptrThing -> attackable -> setMaxHealth(max_health);

    return 0;
}

int ScriptedThing::LoseItem(lua_State *L)
{
    assert(lua_isuserdata(L, 1));
    
    Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

    Thing * ptrThingItem = (Thing *)lua_touserdata(L, 2);

    auto item = std::find_if( ptrThing -> physical -> tInventory.begin(), ptrThing -> physical -> tInventory.end(), 
            [&ptrThingItem](std::shared_ptr<Thing>& p) {return p.get() == ptrThingItem;});

    ptrThing -> physical -> loseItem(*item);

    return 0;
}

int ScriptedThing::GetThing(lua_State *L)
{
    Thing * ptrThing = (Thing *)lua_touserdata(L, 1);

    auto thing = ptrThing -> physical -> getRoom() -> listThings.front();

    ptrThing -> physical -> getRoom() -> listThings.pop_front();

    lua_pushlightuserdata(L, thing.get());

    return 1;

}


void ScriptedThing::InitLua()
{
    luaL_openlibs(L);
    
    luaL_newmetatable(L, "Gauzarbeit.Thing");

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    const luaL_Reg thingMethods[] = {
    {"__index", ScriptedThing::Index},
    {"__newindex", ScriptedThing::NewIndex},
    {"getName", ScriptedThing::GetName},
    {"setMaxHealth", ScriptedThing::SetMaxHealth},
    {"sendMessage", ScriptedThing::SendMessage},
    {"loseItem", ScriptedThing::LoseItem},
    {"getThing", ScriptedThing::GetThing},
    {NULL, NULL}
    };

    luaL_setfuncs(L, thingMethods, 0);
}

lua_State * ScriptedThing::L = luaL_newstate(); 

const std::string ScriptedThing::scriptDir{"./Scripts/"};
