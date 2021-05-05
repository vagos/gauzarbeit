#include "Thing.hpp"
#include "Room.hpp"
#include <memory>

void Physical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (currentRoom) currentRoom -> removeThing(owner);

    currentRoom = Room::get(x, y); 

    currentRoom -> addThing(owner);
}

const std::string Thing::getInfo() const
{
    
    return "";

}
