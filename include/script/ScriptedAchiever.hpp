#ifndef SCRIPTED_ACHIEVER_HPP
#define SCRIPTED_ACHIEVER_HPP

#include "LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Thing.hpp"

#include <string>

class ScriptedAchiever : public Achiever
{
  public:
    struct ScriptedStat : public Achiever::Stat
    {
        ScriptedStat() : Stat() {}

        ScriptedStat(const std::string& s_name) : Stat(), name(s_name) {}

        const std::string getName() override
        {
            auto& L = ScriptedThing::L;

            lua_getglobal(L, "Gauzarbeit");
            lua_getfield(L, -1, "Stats");
            lua_getfield(L, -1, name.c_str());

            if (!lua_isfunction(L, -1))
            {
                return std::to_string(value);
            }

            lua_pushnumber(L, value);
            CheckLua(L, lua_pcall(L, 1, 1, 0));

            return std::string(lua_tostring(L, -1));
        }

        std::string name;
    };
};

#endif // SCRIPTED_ACHIEVER_HPP
