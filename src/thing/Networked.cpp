#include "thing/Networked.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"

void Networked::doDisconnect(const std::shared_ptr<Thing>& owner)
{
    if (owner->_physical && owner->physical()->current_room)
    {
        owner->physical()->current_room->removePlayer(owner);
    }

    setOnline(false);
    Log("Player disconnected!");
}

std::fstream Networked::db;
