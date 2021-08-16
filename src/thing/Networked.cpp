#include "thing/Networked.hpp"
#include "thing/Thing.hpp"
#include "Room.hpp"

void Networked::doDisconnect(const std::shared_ptr<Thing> &owner)
{
    socket -> close();

    assert(owner -> _physical -> current_room);

    owner -> physical() -> current_room -> removePlayer(owner);

    setOnline(false);

    std::clog << "Player disconnected!\n"; 
}

std::size_t Networked::lastID = 0;

std::fstream Networked::db;
