#include "thing/Thing.hpp"
#include "Room.hpp"
#include "Helpers.hpp"

void Physical::dropItem(std::shared_ptr<Thing> item) 
{
        assert(current_room);

        current_room -> addThing(item);
        loseItem(item);
}

std::shared_ptr<Thing> Physical::getItem(std::string item_name)
{ 
    return FindByName(inventory, item_name);
}

void Physical::giveItem(std::shared_ptr<Thing> target, std::shared_ptr<Thing> item)
{
    target -> physical() -> gainItem(item);
    loseItem(item);
}

void Physical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room) current_room -> removeThing(owner);

    current_room = Room::get(x, y); 

    current_room -> addThing(owner);
}

void Physical::pickupItem(std::shared_ptr<Thing> item)
{
   assert(current_room);

   if (!item -> physical() -> is_movable()) return;
 
   current_room -> removeThing(item); 
   gainItem(item);
}
