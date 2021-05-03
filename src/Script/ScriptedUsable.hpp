#include "../Thing.hpp"
#include "ScriptedThing.hpp"
#include "LuaHelpers.hpp"

class ScriptedUsable : public Usable
{
public:
    ScriptedUsable()
    {
    }

    void doUse(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> user) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        lua_getglobal(  L, owner -> sName.c_str() );

        lua_getfield( L, -1, "doUse" );

        lua_pushvalue(L, -2);

        CheckLua(L, lua_pcall(L, 1, 0, 0));
    }

private:

    int lua_doUse(lua_State * L)
    {
        return 0;
    }


};
