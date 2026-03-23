#include "thing/LLMNotifier.hpp"
#include "Helpers.hpp"
#include "Room.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <sstream>

LLMNotifier::LLMNotifier(int cooldown_ticks, int memory_turns, LLMSystem* llm_system)
    : cooldown_ticks(cooldown_ticks), memory_turns(memory_turns), llm_system(llm_system)
{
}

void LLMNotifier::onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                           Event::Type notification_type, const std::shared_ptr<Thing>& target)
{
    if (!owner || !actor || owner == actor)
        return;

    if (notification_type == Event::Type::Chat)
    {
        if (std::dynamic_pointer_cast<LLMNotifier>(actor->_notifier))
            return;

        const std::string payload = buildChatPayload(actor);
        if (payload.empty())
            return;

        latest_stimulus = Stimulus{notification_type, actor->name, payload, ""};
        remember("user: " + actor->name + ": " + payload);
        return;
    }

    if (notification_type == Event::Type::Ask)
    {
        if (!target || target.get() != owner.get())
            return;

        const std::string payload = buildAskPayload(actor);
        latest_stimulus = Stimulus{notification_type, actor->name, payload, target->name};
        remember("user: " + actor->name + ": " + payload);
    }
}

void LLMNotifier::doUpdate(const std::shared_ptr<Thing>& owner)
{
    if (!owner || !owner->_physical || !owner->physical()->current_room)
        return;

    if (!llm_system)
        return;

    if (waiting_result)
    {
        auto maybe_result = llm_system->pollResult(owner->getID());
        if (maybe_result)
        {
            waiting_result = false;
            ticks_until_next = cooldown_ticks;

            const std::string reply = sanitizeReply(*maybe_result);
            if (!reply.empty())
            {
                remember("assistant: " + reply);
                broadcast(owner, reply);
            }
        }
    }

    if (waiting_result)
        return;

    if (ticks_until_next > 0)
    {
        --ticks_until_next;
        return;
    }

    if (!latest_stimulus)
        return;

    const std::string prompt = buildPrompt(owner, *latest_stimulus);
    if (prompt.empty())
        return;

    if (llm_system->enqueueOrReplace(owner->getID(), prompt))
    {
        latest_stimulus.reset();
        waiting_result = true;
    }
    else
    {
        ticks_until_next = cooldown_ticks;
    }
}

std::string LLMNotifier::buildPrompt(const std::shared_ptr<Thing>& owner,
                                     const Stimulus& stimulus) const
{
    if (!owner || !owner->_physical || !owner->physical()->current_room)
        return "";

    auto room = owner->physical()->current_room;

    std::stringstream prompt;
    prompt << "You are " << owner->name
           << ", an NPC in a multiplayer text game. "
              "Reply with one short in-world line. No markdown.\n";

    prompt << "Room: " << room->name << " at (" << room->x << "," << room->y << ").\n";
    prompt << "Speaker: " << stimulus.actor_name << "\n";
    prompt << "Message: " << stimulus.payload << "\n";

    if (!memory.empty())
    {
        prompt << "Recent conversation:\n";
        for (const auto& line : memory)
            prompt << line << "\n";
    }

    prompt << "Reply:";

    return prompt.str();
}

std::string LLMNotifier::sanitizeReply(const std::string& raw) const
{
    std::string reply = raw;

    const std::size_t nl = reply.find('\n');
    if (nl != std::string::npos)
        reply = reply.substr(0, nl);

    boost::algorithm::trim(reply);

    std::string clean;
    clean.reserve(reply.size());
    for (char ch : reply)
    {
        if (!std::iscntrl(static_cast<unsigned char>(ch)))
            clean.push_back(ch);
    }

    boost::algorithm::trim(clean);
    if (clean.size() > 220)
        clean.resize(220);

    return clean;
}

void LLMNotifier::remember(const std::string& line)
{
    if (line.empty())
        return;

    memory.push_back(line);

    const std::size_t max_entries = static_cast<std::size_t>(std::max(1, memory_turns * 2));
    while (memory.size() > max_entries)
    {
        memory.pop_front();
    }
}

void LLMNotifier::broadcast(const std::shared_ptr<Thing>& owner, const std::string& reply) const
{
    if (!owner || !owner->_physical || !owner->physical()->current_room || reply.empty())
        return;

    auto room = owner->physical()->current_room;

    std::stringstream line;
    line << "> " << owner->name << ": " << reply << '\n';

    for (const auto& player : room->players)
    {
        if (!player || player == owner || !player->_networked)
            continue;

        player->networked()->addResponse(line.str());
    }

    owner->notifier()->setEventPayload(reply);
    for (const auto& thing : room->things)
    {
        if (!thing || thing == owner || !thing->_notifier)
            continue;

        thing->notifier()->onNotify(thing, owner, Event::Type::Chat);
    }
}

std::string LLMNotifier::buildAskPayload(const std::shared_ptr<Thing>& actor) const
{
    std::string payload = actor->notifier()->event.object;
    if (!actor->notifier()->event.extra.empty())
    {
        if (!payload.empty())
            payload += " ";
        payload += actor->notifier()->event.extra;
    }

    boost::algorithm::trim(payload);
    if (payload.empty())
        payload = actor->name + " asks to talk.";
    else
        payload = actor->name + " asks about " + payload + ".";

    return payload;
}

std::string LLMNotifier::buildChatPayload(const std::shared_ptr<Thing>& actor) const
{
    std::string payload = actor->notifier()->event.payload;

    if (payload.empty())
    {
        payload = actor->notifier()->event.target;

        if (!actor->notifier()->event.object.empty())
        {
            if (!payload.empty())
                payload += " ";
            payload += actor->notifier()->event.object;
        }

        if (!actor->notifier()->event.extra.empty())
        {
            if (!payload.empty())
                payload += " ";
            payload += actor->notifier()->event.extra;
        }
    }

    boost::algorithm::trim(payload);
    return payload;
}
