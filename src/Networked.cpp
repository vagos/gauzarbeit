#include "Networked.hpp"
#include "World.hpp"
#include <bits/c++config.h>

void Networked::doDisconnect(const std::shared_ptr<Thing> &owner)
{
    socket -> disconnect();
    owner -> physical() -> current_room -> removePlayer(owner);
    online = false;
}

std::size_t Networked::lastID = 0;
