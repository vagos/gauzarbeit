#include "LuaHelpers.hpp"
#include "ScriptedThing.hpp"
#include "ScriptedUsable.hpp"


ScriptedThing::ScriptedThing(const std::string& name):
    Thing(name) 
{
        luaL_openlibs(ScriptedThing::L); // REMOVE THIS
        
        lua_newtable(L);
        lua_setglobal(L, sName.c_str()); // Create a Lua table.

        std::string filename( sName + ".lua" );

        CheckLua(L, luaL_dofile(L, filename.c_str() ));

        lua_getglobal(L, sName.c_str()); // Get the Lua table.
    
        usable = std::make_shared<ScriptedUsable>();
}

lua_State * ScriptedThing::L = luaL_newstate(); 
