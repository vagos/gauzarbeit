#include "player/PlayerNotifier.hpp"
#include "player/Player.hpp"
#include <memory>

void PlayerNotifier::doNotify(const std::shared_ptr<Thing>& owner, const Event& event,
                              const std::shared_ptr<Thing>& target)
{
    for (auto& thing : owner->physical()->current_room->things)
    {
        if (thing->_notifier)
        {
            thing->notifier()->onNotify(thing, owner, event, target);
        }
    }

    for (auto& p : owner->physical()->getRoom()->players)
    {
        if (p == owner)
            continue;

        p->notifier()->onNotify(p, owner, event, target);
    }

    for (auto& t : owner->physical()->inventory)
    {
        if (t->_notifier)
            t->notifier()->onNotify(t, owner, event, target);
    }

    if (owner->_tasker)
        owner->tasker()->onNotify(owner, owner, event, target);

    if (owner->talker()->guild)
        owner->talker()->guild->onNotify(owner, event, target);
}

void PlayerNotifier::onNotify(const std::shared_ptr<Thing>& owner,
                              const std::shared_ptr<Thing>& actor, const Event& event,
                              const std::shared_ptr<Thing>& target)
{
    switch (event.type)
    {

    case Event::Type::Info: // The player passes the event info into their inventory items.
    {
        for (const auto& t : owner->physical()->inventory)
        {
            if (t->_notifier)
                t->notifier()->onNotify(t, actor, event, target);
        }
        break;
    }

    case Event::Type::Chat:
    {
        std::stringstream chat;
        chat << PromptReset << actor->name << ": " << event.payload << '\n';
        owner->networked()->addResponse(chat.str());
        break;
    }

    case Event::Type::Message:
    {
        owner->networked()->addResponse(std::string(PromptReset) + event.payload);
        break;
    }

    case Event::Type::Kill:
    {
        if (actor != owner)
            break;

        std::stringstream msg;
        msg << "You killed " << target->name << '\n';
        owner->networked()->addResponse(ColorString(msg.str(), Color::Red));

        doNotify(owner, event, target);
        owner->achiever()->getRewards(owner, target->achiever()->getLevel());

        break;
    }

    default:
        break;
    }
}
