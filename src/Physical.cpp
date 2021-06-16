#include "Physical.hpp"
#include "Thing.hpp"
#include "Room.hpp"


void Physical::dropItem(std::shared_ptr<Thing> item) 
{
        assert(current_room);

        current_room -> addThing(item);
        loseItem(item);
}

std::shared_ptr<Thing> Physical::getItem(std::string item_name)
{
    auto r = std::find_if(inventory.begin(), inventory.end(), 
            [&item_name](const std::shared_ptr<Thing>& t) {return (t -> name == item_name);}); 

    return r != inventory.end() ? *r : nullptr;
}

void Physical::giveItem(std::shared_ptr<Thing> target, std::shared_ptr<Thing> item)
{
    target -> physical -> gainItem(item);
    loseItem(item);
}

void Physical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room) current_room -> removeThing(owner);

    current_room = Room::get(x, y); 

    current_room -> addThing(owner);
}
