#include "../Thing.hpp"
#include "ScriptedThing.hpp"
#include <lua.h>
#include "LuaHelpers.hpp"


class ScriptedTasker : public Tasker
{
public: 
    ScriptedTasker()
    {
    }

    void giveRewards(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer) override
    {
        //Tasker::giveRewards(owner, completer);

        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "giveRewards" );

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, completer.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

    }

};
