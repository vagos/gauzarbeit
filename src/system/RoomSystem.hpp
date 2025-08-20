#pragma once

#include "System.hpp"
#include "World.hpp"

struct RoomSystem : System
{
    void doUpdate(World& world) override
    {
        for (auto& [id, room] : Room::mapRooms)
        {
            room->doUpdate(world);
        }
    }
};
