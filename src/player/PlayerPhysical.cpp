#include "player/PlayerPhysical.hpp"
#include "Helpers.hpp"
#include "World.hpp"
#include "player/PlayerNetworked.hpp"
#include "script/ScriptedThing.hpp"
#include <memory>
#include <sstream>

void PlayerPhysical::doUpdate(const std::shared_ptr<Thing>& owner, World& world)
{

}

void PlayerPhysical::moveDirection(std::shared_ptr<Thing> owner, const std::string& direction)
{

    if (direction == "left")
    {
        doMove(owner, current_room->x - 1, current_room->y);
    }

    else if (direction == "right")
    {
        doMove(owner, current_room->x + 1, current_room->y);
    }

    else if (direction == "up")
    {
        doMove(owner, current_room->x, current_room->y + 1);
    }

    else if (direction == "down")
    {
        doMove(owner, current_room->x, current_room->y - 1);
    }

    else
        return;

    std::stringstream res;

    res << "Moved into Room: " << current_room->x << " " << current_room->y << "\n";

    owner->networked()->addResponse(res.str());
}

void PlayerPhysical::doMove(std::shared_ptr<Thing> owner, int x, int y)
{
    if (current_room)
        current_room->removePlayer(owner);

    Log("Moving player " << owner->name << " to room " << x << " " << y);
    current_room = Room::get(x, y);
    current_room->addPlayer(owner);
}
