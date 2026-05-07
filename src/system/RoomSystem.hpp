#pragma once

#include "Room.hpp"
#include "System.hpp"
#include "World.hpp"
#include <vector>

struct RoomSystem : System
{
    void doUpdate(World& world) override
    {
        std::vector<std::shared_ptr<Room>> rooms;
        rooms.reserve(Room::mapRooms.size());
        for (const auto& [id, room] : Room::mapRooms)
        {
            (void)id;
            if (room)
                rooms.push_back(room);
        }

        for (const auto& room : rooms)
        {
            room->doUpdate(world);
        }
    }
};
