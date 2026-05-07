#include "script/lua/ScriptedThing.hpp"
#include "Helpers.hpp"
#include "Quest.hpp"
#include "Room.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "script/ScriptAPI.hpp"
#include "script/ScriptPaths.hpp"
#include "script/lua/LuaHelpers.hpp"
#include <cassert>
#include <memory>
#include <string>

namespace
{
std::shared_ptr<Room> FindRoomByThingPtr(Thing* thing_ptr)
{
    for (const auto& [_, room] : Room::mapRooms)
    {
        if (reinterpret_cast<Thing*>(room.get()) == thing_ptr)
            return room;
    }

    return nullptr;
}

int PushThingList(lua_State* L, const std::vector<std::shared_ptr<Thing>>& things)
{
    lua_newtable(L);

    int index = 1;
    for (const auto& thing : things)
    {
        if (!thing)
            continue;

        lua_pushinteger(L, index++);
        lua_pushlightuserdata(L, thing.get());
        lua_settable(L, -3);
    }

    return 1;
}

std::shared_ptr<Thing> ResolveThingTarget(Thing* owner_ptr, Thing* target_ptr)
{
    if (!owner_ptr || !target_ptr || !owner_ptr->_physical)
        return nullptr;

    if (owner_ptr == target_ptr)
        return owner_ptr->shared_from_this();

    auto& current_room = owner_ptr->physical()->current_room;
    if (current_room)
    {
        if (auto target = GetSmartPtr(current_room->players, target_ptr))
            return target;
        if (auto target = GetSmartPtr(current_room->things, target_ptr))
            return target;
    }

    if (auto target = GetSmartPtr(owner_ptr->physical()->inventory, target_ptr))
        return target;

    return nullptr;
}

class ScriptedThinker : public Thinker
{
  public:
    void doThink(const std::shared_ptr<Thing>& owner, World& world) override
    {
        (void)world;

        const auto& L = ScriptedThing_Lua::L;
        // Restore the Lua stack before returning.
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onThink");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return;
        }

        lua_pushlightuserdata(L, owner.get());
        CheckLua(L, lua_pcall(L, 1, 0, 0));
        lua_settop(L, base_top);
    }
};

class ScriptedAchiever : public Achiever
{
  public:
    struct ScriptedStat : public Achiever::Stat
    {
        explicit ScriptedStat(const std::string& s_name) : name(s_name) {}

        const std::string getName() override
        {
            auto& L = ScriptedThing_Lua::L;
            const int base_top = lua_gettop(L);

            lua_getglobal(L, "Gauzarbeit");
            lua_getfield(L, -1, "Stats");
            lua_getfield(L, -1, name.c_str());

            if (!lua_isfunction(L, -1))
            {
                lua_settop(L, base_top);
                return Achiever::Stat::getName();
            }

            lua_pushnumber(L, value);
            CheckLua(L, lua_pcall(L, 1, 1, 0));

            std::string result(lua_tostring(L, -1));
            lua_settop(L, base_top);
            return result;
        }

        std::string name;
    };

    void setStat(const std::string& s_n, float val) override
    {
        if (!extra_stats[s_n])
            extra_stats[s_n] = std::make_shared<ScriptedStat>(s_n);

        extra_stats[s_n]->value = val;
    }
};

class ScriptedUsable : public Usable
{
  public:
    void onUse(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& user) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onUse");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return;
        }

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, user.get());

        CheckLua(L, lua_pcall(L, 2, 0, 0));
        lua_settop(L, base_top);
    }
};

class ScriptedAttackable : public Attackable
{
  public:
    void onAttack(const std::shared_ptr<Thing>& owner,
                  const std::shared_ptr<Thing>& attacker) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onAttack");

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, attacker.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_settop(L, base_top);
        Attackable::onAttack(owner, attacker);
    }

    void doAttack(const std::shared_ptr<Thing>& owner,
                  const std::shared_ptr<Thing>& target) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "doAttack");

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, target.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_settop(L, base_top);
        Attackable::doAttack(owner, target);
    }
};

class ScriptedNotifier : public Notifier
{
    void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                  Event::Type notification_type, const std::shared_ptr<Thing>& target) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onNotify");
        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return;
        }

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, actor.get());
        lua_pushnumber(L, (int)notification_type);
        lua_pushlightuserdata(L, target.get());

        CheckLua(L, lua_pcall(L, 4, 0, 0));
        lua_settop(L, base_top);
    }
};

class ScriptedTasker : public Tasker
{
  public:
    void doReward(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "doReward");

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, completer.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_settop(L, base_top);
    }
};

class ScriptedPhysical : public Physical
{
};

class ScriptedInspectable : public Inspectable
{
    const std::string onInspect(const std::shared_ptr<Thing>& owner,
                                const std::shared_ptr<Thing>& inspector) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onInspect");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return Inspectable::onInspect(owner, inspector);
        }

        lua_pushlightuserdata(L, owner.get());
        lua_pushlightuserdata(L, inspector.get());
        CheckLua(L, lua_pcall(L, 2, 1, 0));

        assert(lua_isstring(L, -1));

        std::string result =
            Inspectable::onInspect(owner, inspector) + std::string(lua_tostring(L, -1));
        lua_settop(L, base_top);
        return result;
    }

    const std::string onHelp(std::shared_ptr<Thing> owner,
                             std::shared_ptr<Thing> inspector) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onHelp");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return Inspectable::getName(owner);
        }

        lua_pushlightuserdata(L, owner.get());
        CheckLua(L, lua_pcall(L, 1, 1, 0));

        assert(lua_isstring(L, -1));
        std::string result(lua_tostring(L, -1));
        lua_settop(L, base_top);
        return result;
    }

    const std::string getName(const std::shared_ptr<Thing>& owner) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "getName");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return Inspectable::getName(owner);
        }

        lua_pushlightuserdata(L, owner.get());
        CheckLua(L, lua_pcall(L, 1, 1, 0));

        assert(lua_isstring(L, -1));
        std::string result(lua_tostring(L, -1));
        lua_settop(L, base_top);
        return result;
    }
};

class ScriptedTalker : public Talker
{
  public:
    void onTalk(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing> talker) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "onTalk");

        if (lua_isfunction(L, -1))
        {
            lua_pushlightuserdata(L, owner.get());
            lua_pushlightuserdata(L, talker.get());
            CheckLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_settop(L, base_top);
        Talker::onTalk(owner, talker);
    }
};

class ScriptedNetworked : public Networked
{
    void doDatabaseLoad(std::shared_ptr<Thing> owner) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "doDatabaseLoad");

        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return;
        }

        lua_pushlightuserdata(L, owner.get());
        CheckLua(L, lua_pcall(L, 1, 0, 0));
        lua_settop(L, base_top);
    }

    const std::string doDatabaseSave(std::shared_ptr<Thing> owner) override
    {
        const auto& L = ScriptedThing_Lua::L;
        const int base_top = lua_gettop(L);

        lua_getglobal(L, owner->name.c_str());
        lua_getfield(L, -1, "doDatabaseSave");
        if (!lua_isfunction(L, -1))
        {
            lua_settop(L, base_top);
            return "";
        }

        lua_pushlightuserdata(L, owner.get());
        CheckLua(L, lua_pcall(L, 1, 1, 0));
        assert(lua_isstring(L, -1));

        std::string result(lua_tostring(L, -1));
        lua_settop(L, base_top);
        return result;
    }
};
} // namespace

ScriptedThing_Lua::ScriptedThing_Lua(const std::string& name, const std::string& script_dir)
    : script::ScriptedThing(name)
{
    const int base_top = lua_gettop(L);

    script_language = ScriptLanguage::Lua;

    // Create components
    _usable = std::make_unique<ScriptedUsable>();
    _attackable = std::make_unique<ScriptedAttackable>();
    _notifier = std::make_unique<ScriptedNotifier>();
    _tasker = std::make_unique<ScriptedTasker>();
    _physical = std::make_unique<ScriptedPhysical>();
    _inspectable = std::make_unique<ScriptedInspectable>();
    _talker = std::make_unique<ScriptedTalker>();
    _thinker = std::make_unique<ScriptedThinker>();
    _achiever = std::make_unique<ScriptedAchiever>();
    _networked = std::make_unique<ScriptedNetworked>();

    lua_getglobal(L, name.c_str());

    if (lua_isnil(L, -1))
    {
        lua_newtable(L);
        lua_setglobal(L, name.c_str()); // Create a Lua table.
    }

    std::string filename = ScriptPaths::ResolveFromDir(script_dir, name, "lua");

    lua_newtable(L);

    luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
    // Keep a lookup from Thing* to Lua registry table ref for lightuserdata dispatch.
    luaRefs[this] = luaRef;

    lua_pushlightuserdata(L, static_cast<Thing*>(this));

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    lua_setmetatable(L, -2);

    CheckLua(L, luaL_dofile(L, filename.c_str()));

    // doInit
    lua_getglobal(L, name.c_str());
    lua_getfield(L, -1, "doInit");

    if (lua_isfunction(L, -1))
    {
        lua_pushlightuserdata(L, static_cast<Thing*>(this));

        CheckLua(L, lua_pcall(L, 1, 0, 0));
    }

    lua_settop(L, base_top);
}

ScriptedThing_Lua::~ScriptedThing_Lua()
{
    auto ref_it = luaRefs.find(this);
    if (ref_it != luaRefs.end())
    {
        // Release the Lua registry reference when the C++ owner dies.
        luaL_unref(L, LUA_REGISTRYINDEX, ref_it->second);
        luaRefs.erase(ref_it);
    }
}

int ScriptedThing_Lua::GetLuaRef(const Thing* thing)
{
    auto ref_it = luaRefs.find(thing);
    if (ref_it == luaRefs.end())
        return LUA_NOREF;

    return ref_it->second;
}

int ScriptedThing_Lua::Index(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    if (!ptrThing)
        return 0;

    if (!lua_isstring(L, 2))
        return 0;

    const char* index = lua_tostring(L, 2);

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    lua_getfield(L, -1, index);

    if (!lua_isnil(L, -1))
        return 1;

    lua_pop(L, 1);

    if (ptrThing->script_language != Thing::ScriptLanguage::Lua)
        return 0;

    const int lua_ref = GetLuaRef(ptrThing);
    if (lua_ref == LUA_NOREF)
        return 0;

    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
    lua_getfield(L, -1, index);

    if (!lua_isnil(L, -1))
        return 1;

    lua_pop(L, 2);

    lua_getglobal(L, ptrThing->name.c_str());
    lua_getfield(L, -1, index);

    if (lua_isnil(L, -1))
        return 0;

    return 1;
}

int ScriptedThing_Lua::NewIndex(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    if (!ptrThing || ptrThing->script_language != Thing::ScriptLanguage::Lua)
    {
        // TODO: Support cross-language writes (e.g. Lua writing fields on JS-backed things).
        return 0;
    }

    const int lua_ref = GetLuaRef(ptrThing);
    if (lua_ref == LUA_NOREF)
        return 0;

    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, -3);

    return 0;
}

int ScriptedThing_Lua::GetName(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    lua_pushstring(L, ptrThing->inspectable()->getName(ptrThing->shared_from_this()).c_str());
    return 1;
}

int ScriptedThing_Lua::SendMessage(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Log(ptrThing->name);

    std::size_t s_l;
    const char* s = lua_tolstring(L, 2, &s_l);

    std::string message{s, s_l};

    ptrThing->networked()->addResponse(message);

    return 0;
}

int ScriptedThing_Lua::LoseItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    ptrThing->physical()->loseItem(item);

    return 0;
}

int ScriptedThing_Lua::DropItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    ptrThing->physical()->dropItem(item);

    return 0;
}

int ScriptedThing_Lua::GetThing(lua_State* L) // Return a thing from inside the room.
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isstring(L, 2));
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    auto thing = ptrThing->physical()->current_room->getThing(lua_tostring(L, 2));

    if (!thing)
        return 0;

    lua_pushlightuserdata(L, thing.get());

    return 1;
}

int ScriptedThing_Lua::GetRoom(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (auto room = FindRoomByThingPtr(ptrThing))
    {
        lua_pushlightuserdata(L, room.get());
        return 1;
    }

    if (!ptrThing->_physical || !ptrThing->physical()->current_room)
        return 0;

    lua_pushlightuserdata(L, ptrThing->physical()->current_room.get());
    return 1;
}

int ScriptedThing_Lua::GetThings(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    auto room = FindRoomByThingPtr((Thing*)lua_touserdata(L, 1));
    if (!room)
        return 0;

    return PushThingList(L, room->things);
}

int ScriptedThing_Lua::GetPlayers(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    auto room = FindRoomByThingPtr((Thing*)lua_touserdata(L, 1));
    if (!room)
        return 0;

    return PushThingList(L, room->players);
}

int ScriptedThing_Lua::GainItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (lua_isstring(L, 2))
    {
        auto t_n = std::string(lua_tostring(L, 2));
        auto item = std::make_shared<ScriptedThing_Lua>(t_n);

        ptrThing->physical()->gainItem(item);
    }

    else if (lua_isuserdata(L, 2))
    {
        Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);
        auto item = GetSmartPtr(ptrThing->physical()->current_room->things, ptrThingItem);

        if (!item)
            return 0;

        ptrThing->physical()->current_room->removeThing(item);
        ptrThing->physical()->gainItem(item);
    }

    return 0;
}

int ScriptedThing_Lua::HasItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (lua_isstring(L, 2))
    {
        auto item = ptrThing->physical()->getItem(lua_tostring(L, 2));
        lua_pushboolean(L, item && ptrThing->physical()->hasItem(item));
        return 1;
    }

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    lua_pushboolean(L, item && ptrThing->physical()->hasItem(item));

    return 1;
}

int ScriptedThing_Lua::MoveTo(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    if (!ptrThing->_physical)
        return 0;

    auto owner = ptrThing->shared_from_this();

    if (lua_isuserdata(L, 2))
    {
        auto room = FindRoomByThingPtr((Thing*)lua_touserdata(L, 2));
        if (!room)
            return 0;

        ptrThing->physical()->doMove(owner, room);
        return 0;
    }

    if (lua_isnumber(L, 2) && lua_gettop(L) < 3)
    {
        if (!ptrThing->physical()->current_room)
            return 0;

        int direction = (int)lua_tonumber(L, 2);
        int x = ptrThing->physical()->current_room->x;
        int y = ptrThing->physical()->current_room->y;

        switch (direction)
        {
        case 0:
            y += 1;
            break;
        case 1:
            y -= 1;
            break;
        case 2:
            x += 1;
            break;
        case 3:
            x -= 1;
            break;
        default:
            return 0;
        }

        ptrThing->physical()->doMove(owner, x, y);
        return 0;
    }

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
        return 0;

    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    ptrThing->physical()->doMove(owner, x, y);
    return 0;
}

int ScriptedThing_Lua::BroadcastMessage(lua_State* L)
{

    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    assert(lua_isstring(L, 2));
    const std::string message{lua_tostring(L, 2)};

    auto p = GetSmartPtr(ptrThing->physical()->getRoom()->players, ptrThing);
    assert(p != nullptr);

    p->notifier()->setEventPayload(message);
    p->notifier()->doNotify(p, Event::Type::Message);

    return 0;
}

int ScriptedThing_Lua::AddTask(lua_State* L)
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isstring(L, 2));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string task_description(lua_tostring(L, 2));

    if (ptrThing->_tasker)
    {
        lua_pushnumber(L, ptrThing->tasker()->addTask(task_description));
        return 1;
    }

    return 0;
}

int ScriptedThing_Lua::TickTask(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_tasker)
    {
        ptrThing->tasker()->tickTask((int)lua_tonumber(L, 2));
    }

    return 0;
}

int ScriptedThing_Lua::GainXP(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_achiever)
    {
        ptrThing->achiever()->gainXP(lua_tonumber(L, 2));
    }

    return 0;
}

int ScriptedThing_Lua::GetLevel(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_achiever)
    {
        lua_pushnumber(L, ptrThing->achiever()->getLevel());

        return 1;
    }

    return 0;
}

int ScriptedThing_Lua::GetEventInfo(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (ptrThing->_notifier)
    {
        lua_newtable(L);

        lua_pushstring(L, ptrThing->notifier()->event.verb.c_str());
        lua_setfield(L, -2, "verb");

        lua_pushstring(L, ptrThing->notifier()->event.target.c_str());
        lua_setfield(L, -2, "target");

        lua_pushstring(L, ptrThing->notifier()->event.object.c_str());
        lua_setfield(L, -2, "object");

        lua_pushstring(L, ptrThing->notifier()->event.extra.c_str());
        lua_setfield(L, -2, "extra");

        return 1;
    }

    return 0;
}

int ScriptedThing_Lua::EquipItem(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    Thing* ptrThingItem = (Thing*)lua_touserdata(L, 2);

    auto item = GetSmartPtr(ptrThing->physical()->inventory, ptrThingItem);

    if (item)
        ptrThing->physical()->equipItem(item);

    return 0;
}

// Returns a Player with the given name in the room the thing is in
int ScriptedThing_Lua::GetPlayer(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    if (!lua_isstring(L, 2))
        return 0;

    std::string t_n(lua_tostring(L, 2));

    Log("NAME: " << t_n);

    auto t = ptrThing->physical()->current_room->getPlayer(t_n);

    if (!t)
        return 0;

    lua_pushlightuserdata(L, t.get());

    return 1;
}

int ScriptedThing_Lua::GainQuest(lua_State* L)
{
    assert(lua_isuserdata(L, 1));

    if (!lua_isstring(L, 2))
        return 0;

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string q_n(lua_tostring(L, 2));

    ptrThing->achiever()->gainQuest(ScriptedQuest(q_n));

    return 0;
}

int ScriptedThing_Lua::DoSay(lua_State* L)
{
    assert(lua_isuserdata(L, 1));
    assert(lua_isuserdata(L, 2));

    if (!lua_isstring(L, 3))
        return 0;

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    Thing* ptrThingTarget = (Thing*)lua_touserdata(L, 2);

    std::string s(lua_tostring(L, 3));

    if (auto room = FindRoomByThingPtr(ptrThingTarget))
    {
        room->onSay(ptrThing->shared_from_this(), s);
        return 0;
    }

    auto target = ResolveThingTarget(ptrThing, ptrThingTarget);
    if (!target || !target->_networked)
        return 0;

    std::stringstream msg;
    msg << ptrThing->name << ": " << s;

    target->networked()->addResponse(msg.str());

    return 0;
}

int ScriptedThing_Lua::DoAttack(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    Thing* ptrThingTarget = (Thing*)lua_touserdata(L, 2);

    if (!(ptrThing->_attackable && ptrThingTarget->_attackable))
        return 0;

    auto t = GetSmartPtr(ptrThing->physical()->current_room->things, ptrThing);
    auto t_t = ptrThing->physical()->current_room->getAnything(ptrThingTarget->name);

    if (!t)
        return 0;

    ptrThing->attackable()->doAttack(t, t_t);

    return 0;
}

int ScriptedThing_Lua::GetStat(lua_State* L)
{
    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);
    std::string s_n(lua_tostring(L, 2));

    float val = ptrThing->achiever()->getStat(s_n);

    lua_pushnumber(L, val);

    return 1;
}

int ScriptedThing_Lua::SetStat(lua_State* L)
{
    assert(lua_isnumber(L, 3));

    Thing* ptrThing = (Thing*)lua_touserdata(L, 1);

    std::string s_n(lua_tostring(L, 2));
    float val = (float)lua_tonumber(L, 3);

    {
        // TODO: These stats probably should be moved to Achiever
        if (s_n == "MaxHealth")
        {
            ptrThing->attackable()->setMaxHealth(val);
        }
        else if (s_n == "Health")
        {
            ptrThing->attackable()->current_health = val;
        }
        else if (s_n == "dmg")
        {
            ptrThing->attackable()->dmg = val;
        }
    }

    ptrThing->achiever()->setStat(s_n, val);

    return 0;
}

int Gauzarbeit_Spawn(lua_State* L)
{
    if (lua_isuserdata(L, 1))
    {
        Room* r = (Room*)lua_touserdata(L, 1);
        std::string t_n(lua_tostring(L, 2));
        auto t = std::make_shared<ScriptedThing_Lua>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, r->x, r->y);
        else
            r->addThing(t);

        return 0;
    }
    else
    {
        int x = (int)lua_tonumber(L, 1);
        int y = (int)lua_tonumber(L, 2);

        std::string t_n(lua_tostring(L, 3));

        auto r = Room::get(x, y);

        assert(r);

        auto t = std::make_shared<ScriptedThing_Lua>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, r->x, r->y);
        else
            r->addThing(t);
    }

    return 0;
}

int Gauzarbeit_Room(lua_State* L)
{
    std::string r_t(lua_tostring(L, 1));

    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);

    auto r = Room::get(x, y);

    r->name = r_t;
    r->doGeneration();

    lua_pushlightuserdata(L, r.get());

    return 1;
}

int Gauzarbeit_ColorString(lua_State* L)
{
    assert(lua_isstring(L, 1));
    assert(lua_isnumber(L, 2));

    std::string s(lua_tostring(L, 1));

    auto c = (Color)lua_tonumber(L, 2);

    lua_pushstring(L, ColorString(s, c).c_str());

    return 1;
}

int Gauzarbeit_LoadDB(lua_State* L)
{
    std::string db_line;

    Networked::getDB() >> db_line;

    lua_pushstring(L, db_line.c_str());

    return 1;
}

int Gauzarbeit_WithChance(lua_State* L)
{
    assert(lua_isnumber(L, 1));

    lua_pushboolean(L, WithChance(lua_tonumber(L, 1)));
    return 1;
}

void ScriptedThing_Lua::Init()
{
    luaL_openlibs(L);

    // Create the Gauzarbeit.Thing metatable
    luaL_newmetatable(L, "Gauzarbeit.Thing");

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    const luaL_Reg thingMethods[] = {{"__index", ScriptedThing_Lua::Index},
                                     {"__newindex", ScriptedThing_Lua::NewIndex},
                                     {"getName", ScriptedThing_Lua::GetName},
                                     {"getType", ScriptedThing_Lua::GetName},
                                     {"setStat", SetStat},
                                     {"getStat", GetStat},
                                     {"sendMessage", ScriptedThing_Lua::SendMessage},
                                     {"doSay", ScriptedThing_Lua::DoSay},
                                     {"loseItem", ScriptedThing_Lua::LoseItem},
                                     {"dropItem", ScriptedThing_Lua::DropItem},
                                     {"equipItem", ScriptedThing_Lua::EquipItem},
                                     {"getThing", ScriptedThing_Lua::GetThing},
                                     {"getPlayer", ScriptedThing_Lua::GetPlayer},
                                     {"getRoom", ScriptedThing_Lua::GetRoom},
                                     {"getThings", ScriptedThing_Lua::GetThings},
                                     {"getPlayers", ScriptedThing_Lua::GetPlayers},
                                     {"gainItem", ScriptedThing_Lua::GainItem},
                                     {"hasItem", ScriptedThing_Lua::HasItem},
                                     {"doMove", ScriptedThing_Lua::MoveTo},
                                     {"broadcastMessage", ScriptedThing_Lua::BroadcastMessage},
                                     {"addTask", ScriptedThing_Lua::AddTask},
                                     {"tickTask", ScriptedThing_Lua::TickTask},
                                     {"gainXP", ScriptedThing_Lua::GainXP},
                                     {"getEventInfo", ScriptedThing_Lua::GetEventInfo},
                                     {"getLevel", ScriptedThing_Lua::GetLevel},
                                     {"gainQuest", ScriptedThing_Lua::GainQuest},
                                     {"doAttack", ScriptedThing_Lua::DoAttack},
                                     {NULL, NULL}};

    luaL_setfuncs(L, thingMethods, 0);

    // Create the global Gauzarbeit table.
    lua_newtable(L);

    // Create Gauzarbeit.Event table
    {
        lua_newtable(L);
        for (const ScriptConstant* c = ScriptAPI::kEventConstants; c->name; ++c)
        {
            lua_pushnumber(L, c->value);
            lua_setfield(L, -2, c->name);
        }
        lua_setfield(L, -2, "Event");
    }

    // Create Gauzarbeit.Color table
    {
        lua_newtable(L);
        for (const ScriptConstant* c = ScriptAPI::kColorConstants; c->name; ++c)
        {
            lua_pushnumber(L, c->value);
            lua_setfield(L, -2, c->name);
        }
        lua_setfield(L, -2, "Color");
    }

    // Create Gauzarbeit.Direction table
    {
        lua_newtable(L);
        for (const ScriptConstant* c = ScriptAPI::kDirectionConstants; c->name; ++c)
        {
            lua_pushnumber(L, c->value);
            lua_setfield(L, -2, c->name);
        }
        lua_setfield(L, -2, "Direction");
    }

    const luaL_Reg gauzarbeitFuncs[] = {{"Spawn", Gauzarbeit_Spawn},
                                        {"GetRoom", Gauzarbeit_Room},
                                        {"ColorString", Gauzarbeit_ColorString},
                                        {"GetDBLine", Gauzarbeit_LoadDB},
                                        {"WithChance", Gauzarbeit_WithChance},
                                        // CreateStat
                                        {NULL, NULL}};

    luaL_setfuncs(L, gauzarbeitFuncs, 0);
    lua_setglobal(L, "Gauzarbeit");
    CheckLua(L, luaL_dofile(L, ScriptPaths::Resolve("Init.lua").c_str()));
    // Load MOTD
    lua_getglobal(L, "MOTD");
    Server::MOTD.assign(lua_tostring(L, -1));

    VerifyLuaAPI(L);
}

lua_State* ScriptedThing_Lua::L = luaL_newstate();
std::unordered_map<const Thing*, int> ScriptedThing_Lua::luaRefs;
