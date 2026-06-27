#include "player/PlayerThinker.hpp"
#include "Helpers.hpp"
#include "Room.hpp"
#include "player/CommandParser.hpp"
#include "player/PlayerPhysical.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Thing.hpp"
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <iomanip>
#include <memory>
#include <sstream>

void PlayerThinker::doThink(const std::shared_ptr<Thing>& owner, World& world)
{
    Thinker::doThink(owner, world);
    const std::string request = owner->networked()->getRequestStream().str();
    if (request.empty())
    {
        return;
    }

    auto event = CommandParser::Parse(request);

    if (event.verb.empty())
    {
        return;
    }

    Log(owner->name << " is thinking: " << event.verb << ' ' << event.target << ' ' << event.object
                    << ' ' << event.extra);

    switch (event.type)
    {
    case Event::Type::Attack:
    {
        auto enemy = owner->physical()->getRoom()->getAnything(event.target);

        if (!enemy)
            throw TargetNotFound(event.target);

        owner->attackable()->doAttack(owner, enemy);

        break;
    }

    case Event::Type::Leave:
    {
        owner->networked()->addResponse("Goodbye!\n");
        owner->networked()->doDatabaseStore(owner);
        owner->networked()->doDisconnect(owner);
        break;
    }

    case Event::Type::Do:
    {
        owner->notifier()->doNotify(owner, event, nullptr);
        break;
    }

    case Event::Type::Help:
    {
        if (event.target.size())
        {
            auto t = owner->physical()->getItem(event.target);

            if (!t)
                throw TargetNotFound(event.target);

            owner->networked()->addResponse(t->inspectable()->getName(t) + ": " +
                                            t->inspectable()->onHelp(t, owner));
        }

        else
        {
            std::stringstream res;
            res << '\n'
                << "A HelpLeaflet materializes in your pocket. Type 'use HelpLeaflet' to read "
                   "it.\n\n";
            owner->physical()->gainItem(ScriptedThing("HelpLeaflet"));
            owner->networked()->addResponse(res.str());
        }

        break;
    }

    case Event::Type::Ask:
    {
        auto t = owner->physical()->current_room->getThing(event.target);

        if (!t)
            throw TargetNotFound(event.target);

        if (event.object.size())
        {
            std::stringstream res;
            res << '\n' << "You ask " << *t << " about " << event.object << '\n';
            owner->networked()->addResponse(ColorString(res.str(), Color::Yellow));
        }

        owner->notifier()->doNotify(owner, event, t);
        t->talker()->onTalk(t, owner, event);

        break;
    }

    case Event::Type::Say:
    {

        std::string message = event.target + ' ' + event.object + ' ' + event.extra;
        boost::algorithm::trim(message);

        Event chat_event(Event::Type::Chat, event.verb, event.target, event.object, event.extra,
                         message);
        owner->notifier()->doNotify(owner, chat_event);

        if (owner->_networked)
        {
            std::stringstream res;
            res << "You said: " << std::quoted(message) << '\n';
            owner->networked()->addResponse(res.str());
        }

        break;

        /*
        auto p = owner -> physical() -> current_room -> getPlayer( event.target );

        if (!p) throw TargetNotFound(event.target);

        std::stringstream whisper; whisper << owner -> name << " whispered to you: " <<
        std::quoted(message) << '\n'; p -> networked() -> addResponse( ColorString( whisper.str(),
        Color::Yellow ) );

        break;
        */
    }
    case Event::Type::Inspect:
    {

        std::shared_ptr<Thing> t;

        if (event.target.empty() || event.target == "here" || event.target == "room")
        {
            t = owner->physical()->current_room;
            auto t = std::reinterpret_pointer_cast<Room>(owner->physical()->current_room);
            owner->networked()->addResponse(t->onInspect(t, owner));
            goto Notify;
        }

        t = owner->physical()->current_room->getAnything(event.target);

        if (t)
        {
            owner->networked()->addResponse(t->inspectable()->onInspect(t, owner));
            goto Notify;
        }

        if (auto task = owner->tasker()->getTask(event.target))
        {
            std::stringstream res;
            res << '[' << (task->tick ? 'x' : ' ') << ']' << ' ' << task->description << '\n';
            owner->networked()->addResponse(res.str());
            goto Notify;
        }

        t = owner->physical()->getItem(event.target);

        if (t)
        {
            owner->networked()->addResponse(t->inspectable()->onInspect(t, owner));
            goto Notify;
        }

        throw TargetNotFound(event.target);

    Notify:

        owner->notifier()->doNotify(owner, event, t);
        break;
    }

    case Event::Type::Use:
    {
        std::shared_ptr<Thing> t;

        if (IsNumber(event.target))
            t = owner->physical()->getItem(std::atoi(event.target.c_str()));
        else
            t = owner->physical()->getItem(event.target);

        if (!t)
        {
            t = owner->physical()->current_room->getThing(event.target);
        }

        if (!t)
            throw TargetNotFound(event.target);

        t->usable()->onUse(t, owner, event);

        break;
    }

    case Event::Type::Move:
    {
        std::static_pointer_cast<PlayerPhysical>(owner->physical())
            ->moveDirection(owner, world, event.target);
        owner->notifier()->doNotify(owner, event);
        break;
    }

    case Event::Type::Gain:
    {
        auto t = owner->physical()->current_room->getThing(event.target);
        if (!t)
            throw TargetNotFound(event.target);
        /* if (!t->physical()->onPickup(t, owner)) */
        /*     throw MissingComponent(); */

        owner->physical()->pickupItem(t);
        owner->notifier()->doNotify(owner, event, t);
        owner->networked()->addResponse("You picked up " + t->name + '\n');
        break;
    }

    case Event::Type::Provide:
    {
        auto o = owner->physical()->getItem(event.object);
        if (!o)
            throw TargetNotFound(event.object);
        auto t = owner->physical()->getRoom()->getThing(event.target);
        if (!t)
            throw TargetNotFound(event.target);
        t->physical()->gainItem(o);
        owner->physical()->loseItem(o);
        owner->notifier()->doNotify(owner, event, t);
        owner->networked()->addResponse("You gave " + t->name + " your " + o->name + "\n");
        break;
    }


    default:
        break;
    }
}
