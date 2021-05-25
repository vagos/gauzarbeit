#include "Networked.hpp"
#include "World.hpp"
#include <bits/c++config.h>

void Networked::doDisconnect(const std::shared_ptr<Thing> &owner)
{
    socket -> disconnect();
    owner -> physical -> getRoom() -> removePlayer(owner);
}

std::size_t Networked::lastID = 0;
