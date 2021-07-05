#include "Room.hpp"
#include "Helpers.hpp"

#include <exception>

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
   return FindByName(things, name);
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
}


const std::string Room::onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector) 
{
    std::stringstream inspect;

    inspect << HeaderString( VerticalListString( players, '*'), "Players here:");
    
    inspect << "\n\n" << CenteredString("---") << "\n\n";

    if (things.size())
    {
        inspect << HeaderString( VerticalListString( things, '*'), "Other things here:");

        inspect << '\n';
    }


    return inspect.str();
}

std::unordered_map< std::int64_t, std::shared_ptr< Room > > Room::mapRooms{};
