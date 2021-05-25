#include "../Thing.hpp"
#include "ScriptedThing.hpp"
#include "LuaHelpers.hpp"

#include <cassert>

class ScriptedUsable : public Usable
{
public:
    ScriptedUsable()
    {
    }

    void onUse(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &user) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        //luaL_getmetatable(L, "Gauzarbeit.Thing");

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "onUse" );

        assert(lua_isfunction(L, -1));

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, user.get());

        CheckLua(L, lua_pcall(L, 2, 0, 0));
    }

};
