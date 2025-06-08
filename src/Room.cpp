#include "Room.hpp"

#include "Helpers.hpp"
#include "script/LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"

#include <cstddef>
#include <exception>
#include <memory>

std::shared_ptr<Room> Room::get(std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        auto newRoom = std::make_shared<ScriptedRoom>(x, y); // Creating BasicRooms for testing.
        Room::mapRooms[key] = newRoom;
    }

    return mapRooms[key];
}

void Room::removePlayer(std::shared_ptr<Thing> player)
{
    players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

void Room::removeThing(std::shared_ptr<Thing> thing)
{
    things.erase(std::remove(things.begin(), things.end(), thing), things.end());
}

void Room::addPlayer(std::shared_ptr<Thing> player)
{
    players.push_back(player);
}

void Room::addThing(std::shared_ptr<Thing> thing)
{
    things.push_back(thing);
}

const std::shared_ptr<Thing> Room::getPlayer(const std::string& name)
{
    return FindByName(players, name);
}

const std::shared_ptr<Thing> Room::getThing(const std::string& name)
{
    return FindByName(things, name);
}

const std::shared_ptr<Thing> Room::getAnything(const std::string& name)
{
    auto t = getThing(name);

    return t ? t : getPlayer(name);
}

void Room::doUpdate(World& world)
{
    for (const auto& t : things)
    {
        try
        {
            t->doUpdate(world);
        }

        catch (std::exception& e)
        {
            std::clog << e.what() << '\n';
        }
    }

    // Remove dead things
    things.erase(std::remove_if(things.begin(), things.end(),
                                [](auto& t)
                                { return t->_attackable && !t->attackable()->is_alive(); }),
                 things.end());
}

const std::string Room::onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector)
{
    std::stringstream inspect;

    inspect << HeaderString("", "Room: " + name) << '\n';

    inspect << HeaderString(BlockListString(players, '*',
                                            [](const std::shared_ptr<Thing>& t)
                                            { return t->inspectable()->getName(t); }),
                            "Players here:");

    inspect << '\n' << CenteredString("---") << "\n\n";

    if (things.size())
    {
        inspect << HeaderString(BlockListString(things, '*',
                                                [](const std::shared_ptr<Thing>& t)
                                                { return t->inspectable()->getName(t); }),
                                "Other things here:");
    }

    return inspect.str();
}

void ScriptedRoom::doGeneration()
{
    const auto& L = ScriptedThing::L;

    lua_getglobal(L, name.c_str());

    if (lua_isnil(L, -1))
    {
        lua_newtable(L);
        lua_setglobal(L, name.c_str()); // Create a Lua table.
    }

    std::string filename("./scripts/rooms/" + name + ".lua");

    CheckLua(L, luaL_dofile(L, filename.c_str()));

    // doInit

    lua_getglobal(L, name.c_str());

    lua_getfield(L, -1, "doInit");

    if (lua_isfunction(L, -1))
    {
        lua_pushlightuserdata(L, this);
        CheckLua(L, lua_pcall(L, 1, 0, 0));
    }

    // Spawn Things

    lua_getglobal(L, "Respawns");

    if (lua_isnil(L, -1))
        return;

    lua_pushnil(L);

    std::vector<std::string> t_ns;

    while (lua_next(L, -2) != 0)
    {
        std::string t_n(lua_tostring(L, -1));

        std::clog << t_n << '\n';

        t_ns.push_back(t_n);

        lua_pop(L, 1);
    }

    for (auto t_n : t_ns)
    {
        auto t = std::make_shared<ScriptedThing>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, x, y);
        else
            addThing(t);
    }
}

void ScriptedRoom::doUpdate(World& world)
{
    const auto& L = ScriptedThing::L;

    Room::doUpdate(world);

    return;

    lua_getglobal(L, name.c_str());

    if (lua_isnil(L, -1))
        return;

    lua_getfield(L, -1, "doUpdate");

    if (!lua_isfunction(L, -1))
        return;

    lua_pushlightuserdata(L, this);
    CheckLua(L, lua_pcall(L, 1, 0, 0));
}

std::unordered_map<std::int64_t, std::shared_ptr<Room>> Room::mapRooms{};
