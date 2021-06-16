#include "Room.hpp"

std::shared_ptr<Room> Room::get(std::int32_t x, std::int32_t y)
{
    std::int64_t key = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms[key])
    {
        auto newRoom = std::make_shared<BasicRoom>(x, y); // Creating BasicRooms for testing.
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
    auto r = std::find_if( players.begin(), players.end(), 
           [&name](const std::shared_ptr<Thing> &t) { return t -> name == name; });
 
    return r != players.end() ? *r : nullptr;
}

const std::shared_ptr<Thing> Room::getThing(const std::string &name)
{
   auto r = std::find_if( things.begin(), things.end(), 
           [&name](const std::shared_ptr<Thing> &t) { return t -> name == name; });
 
   return r != things.end() ? *r : nullptr;
}

void Room::doUpdate(World &world)
{
    for (const auto& t : things)    
    {
        if (t -> attackable) t -> attackable -> doUpdate(t);
    }
}

std::unordered_map< std::int64_t, std::shared_ptr< Room > > Room::mapRooms{};
