#pragma once

#include "Room.hpp"
#include "System.hpp"
#include "World.hpp"
#include <vector>

struct RoomSystem : System
{
    explicit RoomSystem(World& world) : System(world) {}

    void doUpdate(World& world) override
    {
        std::vector<std::shared_ptr<Room>> rooms;
        rooms.reserve(Room::mapRooms.size());
        for (const auto& [_, room] : Room::mapRooms)
        {
            if (room)
                rooms.push_back(room);
        }

        for (const auto& room : rooms)
        {
            room->doUpdate(world);
        }
    }
};
