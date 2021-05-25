#include "Physical.hpp"
#include "Thing.hpp"
#include "Room.hpp"


void Physical::dropItem(std::shared_ptr<Thing> item) 
{
        current_room -> addThing(item);
        loseItem(item);
}

std::shared_ptr<Thing> Physical::getItem(std::string item_name)
{
    auto r = std::find_if(inventory.begin(), inventory.end(), 
            [&item_name](const std::shared_ptr<Thing>& t) {return (t -> name == item_name);}); 

    return r != inventory.end() ? *r : nullptr;
}
