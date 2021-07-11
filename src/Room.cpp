#include "Room.hpp"
#include "Helpers.hpp"
#include "Script/ScriptedThing.hpp"

#include "Script/LuaHelpers.hpp"

#include <cstddef>
#include <exception>
#include <lua.h>

std::shared_ptr<Room> Room::get(std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        auto newRoom = std::make_shared<Room>(x, y); // Creating BasicRooms for testing.
        Room::mapRooms[key] = newRoom;
        
        newRoom -> doGeneration();
    }
        
    return mapRooms[key]; 
}

std::shared_ptr<Room> Room::get(const std::string &r_t, std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        auto newRoom = std::make_shared<ScriptedRoom>(r_t, x, y); // Creating BasicRooms for testing.
        Room::mapRooms[key] = newRoom;
        
        newRoom -> doGeneration();
    }
        
    return mapRooms[key]; 
}

void Room::removePlayer(std::shared_ptr<Thing> player)
{
    players.erase( std::remove( players.begin(), players.end(), player ), players.end() );
}

void Room::removeThing(std::shared_ptr<Thing> thing)
{
    things.erase( std::remove( things.begin(), things.end(), thing ), things.end() );
}


void Room::addPlayer(std::shared_ptr<Thing> player)
{
    players.push_back(player);
}

void Room::addThing(std::shared_ptr<Thing> thing)
{
    things.push_back(thing);
}

const std::shared_ptr<Thing> Room::getPlayer(const std::string &name)
{
   return FindByName(players, name);
}

const std::shared_ptr<Thing> Room::getThing(const std::string &name)
{
   return FindByName(things, name);
   // auto container = things;
   // auto s = name;

   // auto r = std::find_if(container.begin(), container.end(), 
   //        [&s](const auto& i){return i -> name == s;});


   //return r != container.end() ? *r : nullptr;

}

const std::shared_ptr<Thing> Room::getAnything(const std::string &name)
{
    auto t = getThing(name);

    if (!t) return getPlayer(name);

    return t;
}

void Room::doUpdate(World &world)
{
    for (const auto& t : things)    
    {
        try
        {
            t -> attackable() -> doUpdate(t);
        }

        catch (std::exception& e) 
        {
        
        }
    }

    // Remove dead things

    things.erase( std::remove_if(things.begin(), things.end(), 
                [](auto& t){return t -> _attackable && !t -> attackable() -> is_alive();}), things.end() );

}


const std::string Room::onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector) 
{
    std::stringstream inspect;

    inspect << HeaderString( VerticalListString( players, '*'), "Players here:");
    
    inspect << '\n' << CenteredString("---") << "\n\n";

    if (things.size())
    {
        inspect << HeaderString( VerticalListString( things, '*'), "Other things here:");
    }


    return inspect.str();
}

ScriptedRoom::ScriptedRoom(const std::string& room_type, int x, int y): Room(x, y), room_type(room_type)
{
}

void ScriptedRoom::doGeneration()
{
    const auto& L = ScriptedThing::L;
                                                                       
    lua_getglobal(L, room_type.c_str());

    if (lua_isnil(L, -1))
    {
         lua_newtable(L);
         lua_setglobal(L, room_type.c_str()); // Create a Lua table.
    }

    std::string filename( "./scripts/rooms/" + room_type + ".lua" );

    CheckLua(L, luaL_dofile(L, filename.c_str() ));

    // doInit
    
    lua_getglobal(L, room_type.c_str());

    lua_getfield(L, -1, "doInit");

    if (lua_isfunction(L, -1))
    {
       lua_pushlightuserdata(L, this); 
       CheckLua(L, lua_pcall(L, 1, 0, 0));
    }

    // Spawn Things
    
    lua_getglobal(L, "Respawns");
    
    lua_pushnil(L);

    std::vector<std::string> t_ns;

    while (lua_next(L, -2) != 0)
    {
          std::string t_n( lua_tostring(L, -1) );

          t_ns.push_back(t_n);

          lua_pop(L, 1);
    }

    for (auto t_n : t_ns)
    {
         auto t = std::make_shared<ScriptedThing>(t_n);

         if (t -> _physical) t -> physical() -> doMove(t, x, y);
         else addThing(t);
    }
}

void ScriptedRoom::doUpdate(World &world)
{
    const auto& L = ScriptedThing::L;

    lua_getglobal( L, room_type.c_str() );

    lua_getfield( L, -1, "doUpdate" );

    if (!lua_isfunction(L,-1)) return;

    lua_pushlightuserdata(L, this);
    CheckLua(L, lua_pcall(L, 1, 0, 0));

    Room::doUpdate(world);
}

std::unordered_map< std::int64_t, std::shared_ptr< Room > > Room::mapRooms{};

