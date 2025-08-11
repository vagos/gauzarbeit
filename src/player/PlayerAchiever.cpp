#include "player/PlayerAchiever.hpp"
#include "Quest.hpp"

PlayerAchiever::PlayerAchiever() : Achiever()
{
    setStat("Attack", 1);
    setStat("Defense", 1);
}

void PlayerAchiever::onQuestComplete(const std::shared_ptr<Thing>& owner,
                                     const std::shared_ptr<Thing>& quest)
{
    Achiever::onQuestComplete(owner, quest);

    std::stringstream msg;
    msg << "You completed the Quest: " << std::quoted(quest->name) << "\n\n";

    owner->networked()->addResponse(ColorString(msg.str(), Color::Green));
}

void PlayerAchiever::getRewards(const std::shared_ptr<Thing> owner, int size)
{
    Achiever::getRewards(owner, size);

    std::stringstream msg;
    msg << "You gained " << size << " XP!\n\n";

    owner->networked()->addResponse(msg.str());
}
