#include "player/PlayerAchiever.hpp"
#include "thing/Thing.hpp"

PlayerAchiever::PlayerAchiever() : Achiever()
{
    setStat("Attack", 1);
    setStat("Defense", 1);
}

void PlayerAchiever::getRewards(const std::shared_ptr<Thing> owner, int size)
{
    Achiever::getRewards(owner, size);

    std::stringstream msg;
    msg << "You gained " << size << " XP!\n\n";

    owner->networked()->addResponse(msg.str());
}
