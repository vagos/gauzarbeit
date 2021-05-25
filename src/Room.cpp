#include "Room.hpp"

std::shared_ptr<Room> Room::get(std::int32_t x, std::int32_t y)
{
    std::int64_t nKey = (x & 0xFFFF) << 16 | (y & 0xFFFF);

    if (!mapRooms.contains(nKey))
    {
        auto newRoom = std::make_shared<Room>(x, y);
        Room::mapRooms[nKey] = newRoom;
    }
        
    return mapRooms[nKey]; 
}

void Room::removePlayer(std::shared_ptr<Thing> player)
{
    listPlayers.remove(player);
}

void Room::removeThing(std::shared_ptr<Thing> thing)
{
    thing -> physical -> current_room = nullptr;
    listThings.remove(thing);
}


void Room::addPlayer(std::shared_ptr<Thing> player)
{
    listPlayers.push_back(player);
}

void Room::addThing(std::shared_ptr<Thing> thing)
{
    listThings.push_back(thing);
}

const std::shared_ptr<Thing> Room::getPlayer(const std::string &name)
{
    auto r = std::find_if( listPlayers.begin(), listPlayers.end(), 
           [&name](const std::shared_ptr<Thing> &t) { return t -> name == name; });
 
    return r != listPlayers.end() ? *r : nullptr;
}

const std::shared_ptr<Thing> Room::getThing(const std::string &name)
{
   auto r = std::find_if( listThings.begin(), listThings.end(), 
           [&name](const std::shared_ptr<Thing> &t) { return t -> name == name; });
 
   return r != listThings.end() ? *r : nullptr;
}

void Room::doUpdate(World &world)
{
    
}

std::unordered_map< std::int64_t, std::shared_ptr< Room > > Room::mapRooms{};
