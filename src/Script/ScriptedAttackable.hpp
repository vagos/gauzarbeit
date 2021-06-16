#include "../Thing.hpp"
#include "ScriptedThing.hpp"
#include "LuaHelpers.hpp"

class ScriptedAttackable : public Attackable
{
    
    void onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "onAttack" );

        if (!lua_isfunction(L, -1)) return;

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, attacker.get());
        
        CheckLua(L, lua_pcall(L, 2, 0, 0));

        Attackable::onAttack(owner, attacker);
    }

};
