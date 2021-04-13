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

int x, y;

void Room::removePlayer(std::shared_ptr<Thing> player)
{
    listPlayers.remove(player);
}

void Room::addPlayer(std::shared_ptr<Thing> player)
{
    listPlayers.push_back(player);
}

std::unordered_map< std::int64_t, std::shared_ptr< Room > > Room::mapRooms{};
