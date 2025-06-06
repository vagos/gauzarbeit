#ifndef SCRIPTED_NOTIFIER_HPP
#define SCRIPTED_NOTIFIER_HPP

#include <cassert>

#include "script/LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Thing.hpp"

class ScriptedNotifier : public Notifier
{

	void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
				  Event::Type notification_type, const std::shared_ptr<Thing>& target) override
	{
		const auto& L = ScriptedThing::L;

		lua_getglobal(L, owner->name.c_str());

		lua_getfield(L, -1, "onNotify");

		if (!lua_isfunction(L, -1))
			return;

		lua_pushlightuserdata(L, owner.get());
		lua_pushlightuserdata(L, actor.get());
		lua_pushnumber(L, (int)notification_type);
		lua_pushlightuserdata(L, target.get());

		CheckLua(L, lua_pcall(L, 4, 0, 0));
	}
};

#endif
