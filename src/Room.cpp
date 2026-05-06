#include "Room.hpp"
#include "Helpers.hpp"
#include "script/ScriptedThing.hpp"
#include "script/ScriptPaths.hpp"
#include "script/lua/LuaHelpers.hpp"
#include "script/lua/ScriptedThing.hpp"
#include "system/WorldGenSystem.hpp"
#include "World.hpp"
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

namespace
{
std::filesystem::path RoomDBPath(const Room& room)
{
    std::ostringstream filename;
    filename << room.x << "_" << room.y;
    return std::filesystem::path("./db/rooms") / filename.str();
}
} // namespace

std::shared_ptr<Room> Room::get(std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        auto newRoom = std::make_shared<ScriptedRoom>(x, y);
        Room::mapRooms[key] = newRoom;
        if (newRoom->_networked)
            newRoom->networked()->doDatabaseLoad(newRoom);
    }

    return mapRooms[key];
}

std::shared_ptr<Room> Room::get(World& world, std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        std::ostringstream filename;
        filename << x << "_" << y;
        const auto room_db_path = std::filesystem::path("./db/rooms") / filename.str();

        std::shared_ptr<Room> newRoom;
        if (std::filesystem::exists(room_db_path))
            newRoom = std::make_shared<ScriptedRoom>(x, y);
        else if (auto* worldgen = world.getSystem<WorldGenSystem>())
            newRoom = worldgen->generateRoom(x, y);
        else
            newRoom = std::make_shared<ScriptedRoom>(x, y);

        Room::mapRooms[key] = newRoom;
        if (newRoom->_networked)
            newRoom->networked()->doDatabaseLoad(newRoom);
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
    players.erase(std::remove(players.begin(), players.end(), thing), players.end());
}

void Room::addPlayer(std::shared_ptr<Thing> player)
{
    players.push_back(player);
}

void Room::addThing(std::shared_ptr<Thing> thing)
{
    if (thing->is_player)
    {
        addPlayer(thing);
        return;
    }

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
            Log(e.what());
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

    inspect << "\n\n"
            << "(" << x << ", " << y << ")\n\n";

    return inspect.str();
}

void Room::onSay(const std::shared_ptr<Thing>& speaker, const std::string& message)
{
    if (!speaker)
        return;

    std::stringstream line;
    line << speaker->name << ": " << message;

    for (const auto& player : players)
    {
        if (!player || player == speaker || !player->_networked)
            continue;

        player->networked()->addResponse(line.str());
    }
}

void ScriptedRoom::doGeneration()
{
    if (has_generated)
        return;

    has_generated = true;

    const auto& L = ScriptedThing_Lua::L;

    lua_getglobal(L, name.c_str());

    if (lua_isnil(L, -1))
    {
        lua_newtable(L);
        lua_setglobal(L, name.c_str()); // Create a Lua table.
    }

    std::string filename = ScriptPaths::Resolve("rooms/" + name + ".lua");

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

        Log(t_n);

        t_ns.push_back(t_n);

        lua_pop(L, 1);
    }

    for (auto t_n : t_ns)
    {
        auto t = std::make_shared<ScriptedThing_Lua>(t_n);

        if (t->_physical)
            t->physical()->doMove(t, x, y);
        else
            addThing(t);
    }
}

void ScriptedRoom::doUpdate(World& world)
{
    const auto& L = ScriptedThing_Lua::L;

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

void RoomNetworked::doDatabaseLoad(std::shared_ptr<Thing> owner)
{
    auto room = std::static_pointer_cast<Room>(owner);
    if (!room)
        return;

    const auto filename = RoomDBPath(*room);
    if (!std::filesystem::exists(filename))
        return;

    if (db.is_open())
        db.close();

    db.open(filename, std::ios::in);
    if (!db.is_open())
        return;

    std::string line;
    db >> line;
    if (line == "ROOM:")
    {
        std::getline(db >> std::ws, room->name);
        room->name = TrimLine(room->name);
    }

    db >> line;
    if (line != "THINGS")
    {
        db.close();
        return;
    }

    room->things.clear();

    while (db >> line)
    {
        if (line == "END")
            break;

        try
        {
            auto thing = ScriptedThing(line);
            if (thing->_physical)
                thing->physical()->doMove(thing, room);
            else
                room->addThing(thing);
            if (thing->_networked)
                thing->networked()->doDatabaseLoad(thing);
        }
        catch (std::exception& e)
        {
            // Some runtime-only things (e.g. C++-spawned NPCs) are intentionally not script-backed.
            if (std::string(e.what()).find("not found") == std::string::npos)
                Log("Skipping persisted room thing '" << line << "': " << e.what());
        }
    }

    room->has_generated = true;
    db.close();
}

const std::string RoomNetworked::doDatabaseSave(std::shared_ptr<Thing> owner)
{
    auto room = std::static_pointer_cast<Room>(owner);
    if (!room)
        return "";

    std::stringstream data;
    data << "ROOM: " << room->name << '\n';
    data << "THINGS\n";
    for (const auto& thing : room->things)
    {
        // Persist only script-backed things
        // TODO: Support dynamic loading for compiled-language NPCs without a factory registry.
        if (thing->script_language == Thing::ScriptLanguage::None)
            continue;

        data << thing->name << '\n';
    }
    data << "END\n";

    return data.str();
}

void RoomNetworked::doDatabaseStore(std::shared_ptr<Thing> owner)
{
    auto room = std::static_pointer_cast<Room>(owner);
    if (!room)
        return;

    const std::filesystem::path room_dir("./db/rooms");
    if (!std::filesystem::exists(room_dir))
        std::filesystem::create_directories(room_dir);

    const auto filename = RoomDBPath(*room);

    if (db.is_open())
        db.close();

    db.open(filename, std::ios::out | std::ios::trunc);
    if (!db.is_open())
    {
        std::ofstream file(filename);
        db.open(filename, std::ios::out | std::ios::trunc);
    }
    if (!db.is_open())
        return;

    db << doDatabaseSave(owner);
    db.close();
}
