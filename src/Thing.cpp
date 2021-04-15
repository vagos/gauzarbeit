#include "Thing.hpp"
#include "Room.hpp"
#include <memory>

void Physical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (currentRoom) currentRoom -> removePlayer(owner);

    currentRoom = Room::get(x, y); 

    currentRoom -> addPlayer(owner);
}

const std::string Thing::getInfo() const
{
    
    return "";

}
