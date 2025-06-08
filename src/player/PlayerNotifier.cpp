#include "player/PlayerNotifier.hpp"

#include "player/Player.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <memory>

void PlayerNotifier::setEvent(const std::shared_ptr<Thing>& owner)
{
    std::stringstream req{owner->networked()->getRequestStream().str()};

    // auto words = TokenizeString(req.str());

    /*if (words.size() >= 4 && req.str().find("from") != std::string::npos)
    {
        event.verb = words[0];
        event.object = words[1];
        event.target = words[3];

        return;
    }

    else if (words.size() >= 4 && req.str().find("with") != std::string::npos)
    {
        event.verb = words[0];
        event.target = words[1];
        event.object = words[3];

        return;

    }

    else if (words.size() >= 4 && req.str().find("on") != std::string::npos)
    {
        event.verb = words[0];
        event.target = words[1];
        event.object = words[3];

        return;
    }
    */

    req >> event.verb >> event.target >> event.object;

    std::getline(req >> std::ws, event.extra);

    boost::to_lower(event.verb);

    event.type = Player::playerCommands[event.verb];

    if (event.verb.size() && event.type == Event::Type::Invalid)
        throw InvalidCommand();
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

    for (auto& quest : owner->achiever()->quests)
    {
        quest->notifier()->onNotify(quest, owner, notification_type, target);
    }

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
        chat << "> " << actor->name << ": " << actor->notifier()->event.payload << '\n';
        owner->networked()->addResponse(chat.str());
        break;
    }

    case Event::Type::Message:
    {
        owner->networked()->addResponse(actor->notifier()->event.payload);
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

    case Event::Type::Gain_Quest:
    {
        assert(target);

        if (actor != owner)
            break;

        std::stringstream res;
        res << "Quest " << std::quoted(target->name) << " added to your Quest Log!\n";

        owner->networked()->addResponse(ColorString(res.str(), Color::Green));
    }
    }
}
