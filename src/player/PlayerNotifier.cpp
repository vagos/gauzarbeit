#include "player/PlayerNotifier.hpp"
#include "player/CommandParser.hpp"
#include "player/Player.hpp"
#include <memory>

void PlayerNotifier::setEvent(const std::shared_ptr<Thing>& owner)
{
    event = CommandParser::Parse(owner->networked()->getRequestStream().str());
}

void PlayerNotifier::doNotify(const std::shared_ptr<Thing>& owner, Event::Type notification_type,
                              const std::shared_ptr<Thing>& target)
{
    for (auto& thing : owner->physical()->current_room->things)
    {
        if (thing->_notifier)
        {
            thing->notifier()->onNotify(thing, owner, notification_type, target);
        }
    }

    for (auto& p : owner->physical()->getRoom()->players)
    {
        if (p == owner)
            continue;

        p->notifier()->onNotify(p, owner, notification_type, target);
    }

    for (auto& t : owner->physical()->inventory)
    {
        if (t->_notifier)
            t->notifier()->onNotify(t, owner, notification_type, target);
    }

    if (owner->_tasker)
        owner->tasker()->onNotify(owner, owner, notification_type, target);

    if (owner->talker()->guild)
        owner->talker()->guild->onNotify(owner, notification_type, target);
}

void PlayerNotifier::onNotify(const std::shared_ptr<Thing>& owner,
                              const std::shared_ptr<Thing>& actor, Event::Type notification_type,
                              const std::shared_ptr<Thing>& target)
{
    switch (notification_type)
    {

    case Event::Type::Info: // The player passes the event info into their inventory items.
    {
        for (const auto& t : owner->physical()->inventory)
        {
            if (t->_notifier)
                t->notifier()->onNotify(t, actor, notification_type, target);
        }
        break;
    }

    case Event::Type::Chat:
    {
        std::stringstream chat;
        chat << PromptReset << "> " << actor->name << ": "
             << actor->notifier()->event.payload;
        owner->networked()->addResponse(chat.str());
        break;
    }

    case Event::Type::Message:
    {
        owner->networked()->addResponse(std::string(PromptReset) + actor->notifier()->event.payload);
        break;
    }

    case Event::Type::Kill:
    {
        if (actor != owner)
            break;

        std::stringstream msg;
        msg << "You killed " << target->name << '\n';
        owner->networked()->addResponse(ColorString(msg.str(), Color::Red));

        doNotify(owner, Event::Type::Kill, target);
        owner->achiever()->getRewards(owner, target->achiever()->getLevel());

        break;
    }

    default:
        break;
    }
}
