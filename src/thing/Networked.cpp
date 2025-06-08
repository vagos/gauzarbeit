#include "thing/Networked.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"

void Networked::doDisconnect(const std::shared_ptr<Thing>& owner)
{
	assert(owner->_physical->current_room);
	owner->physical()->current_room->removePlayer(owner);

	setOnline(false);
	std::clog << "Player disconnected!\n";
}

std::size_t Networked::lastID = 0;

std::fstream Networked::db;
