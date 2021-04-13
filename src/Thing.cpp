#include "Thing.hpp"
#include "Room.hpp"

void Physical::doMove(int x, int y)
{
    if (currentRoom) currentRoom -> removePlayer(owner);

    currentRoom = Room::get(x, y); 

    currentRoom -> addPlayer(owner);
}
