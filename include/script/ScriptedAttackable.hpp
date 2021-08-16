#include "thing/Thing.hpp"
#include "script/ScriptedThing.hpp"
#include "script/LuaHelpers.hpp"

class ScriptedAttackable : public Attackable
{

public:
    ScriptedAttackable(): Attackable()
    {
    }
    
    void onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "onAttack" );

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, attacker.get());
            
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        Attackable::onAttack(owner, attacker);
    }

    void doAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &target) override
    {
        const auto& L = std::static_pointer_cast<ScriptedThing>(owner) -> L;

        lua_getglobal(  L, owner -> name.c_str() );

        lua_getfield( L, -1, "doAttack" );

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, target.get());
            
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        Attackable::doAttack(owner, target);
    }

};
