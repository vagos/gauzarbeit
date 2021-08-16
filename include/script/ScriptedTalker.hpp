#ifndef SCRIPTED_TALKER_HPP
#define SCRIPTED_TALKER_HPP

#include "thing/Thing.hpp"
#include "script/ScriptedThing.hpp"
#include "script/LuaHelpers.hpp"

class ScriptedTalker : public Talker
{
public:

    ScriptedTalker()
    {
    }

    void onTalk(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> talker) override
    {
        const auto& L = ScriptedThing::L;

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "onTalk" );

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, talker.get());

            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        Talker::onTalk(owner, talker);
    }

};

#endif
