#include "../Thing.hpp"
#include "ScriptedThing.hpp"
#include "LuaHelpers.hpp"
#include <lua.h>

#include <cassert>

class ScriptedUsable : public Usable
{
public:
    ScriptedUsable()
    {
    }

    void onUse(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> user) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        //luaL_getmetatable(L, "Gauzarbeit.Thing");

        lua_getglobal(  L, owner -> sName.c_str() );

        lua_getfield( L, -1, "onUse" );

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, user.get());

        //luaL_getmetatable(L, "Gauzarbeit.Thing");
        //lua_setmetatable(L, -2);

        CheckLua(L, lua_pcall(L, 2, 0, 0));
    }

};
