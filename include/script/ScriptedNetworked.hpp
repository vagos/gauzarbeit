#ifndef SCRIPTED_NETWORKED_HPP
#define SCRIPTED_NETWORKED_HPP

#include "LuaHelpers.hpp"
#include "ScriptedThing.hpp"

class ScriptedNetworked : public Networked
{

    void doDatabaseLoad(std::shared_ptr<Thing> owner) override
    {
        const auto& L = ScriptedThing::L;

        lua_getglobal(L, owner->name.c_str());

        lua_getfield(L, -1, "doDatabaseLoad");

        if (!lua_isfunction(L, -1))
            return;

        lua_pushlightuserdata(L, owner.get());

        CheckLua(L, lua_pcall(L, 1, 0, 0));
    }

    const std::string doDatabaseSave(std::shared_ptr<Thing> owner) override
    {
        const auto& L = ScriptedThing::L;

        lua_getglobal(L, owner->name.c_str());

        lua_getfield(L, -1, "doDatabaseSave");

        if (!lua_isfunction(L, -1))
            return "";

        lua_pushlightuserdata(L, owner.get());

        CheckLua(L, lua_pcall(L, 1, 1, 0));

        assert(lua_isstring(L, -1));

        return std::string(lua_tostring(L, -1));
    }
};

#endif
