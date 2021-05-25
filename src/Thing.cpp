#include "Thing.hpp"
#include "Room.hpp"
#include <memory>

void Physical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room) current_room -> removeThing(owner);

    current_room = Room::get(x, y); 

    current_room -> addThing(owner);
}

const std::string Thing::getInfo() const
{
    
    return "";

}
