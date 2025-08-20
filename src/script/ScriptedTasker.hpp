#include "script/LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Thing.hpp"

class ScriptedTasker : public Tasker
{
  public:
    ScriptedTasker() {}

    void doReward(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer) override
    {
        // Tasker::giveRewards(owner, completer);

        const auto& L = std::static_pointer_cast<ScriptedThing>(owner)->L;

        lua_getglobal(L, owner->name.c_str());

        lua_getfield(L, -1, "doReward");

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, completer.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }
    }
};
