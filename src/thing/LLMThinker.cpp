#include "thing/LLMThinker.hpp"
#include "Helpers.hpp"
#include "Room.hpp"
#include "World.hpp"
#include "player/CommandParser.hpp"
#include <boost/algorithm/string.hpp>

void LLMThinker::doThink(const std::shared_ptr<Thing>& owner, World& world)
{
    Thinker::doThink(owner, world);

    if (!owner || !llm_system)
        return;

    if (waiting_result)
    {
        auto result = llm_system->pollResult(owner->getID());
        if (!result)
            return;

        Log(owner->name << " received LLM result: " << *result);
        waiting_result = false;
        executeCommand(owner, world, *result);
        next_think_time = world.getCurrentTime() + cooldown_seconds;
        return;
    }

    if (world.getCurrentTime() < next_think_time)
        return;

    if (llm_system->enqueueOrReplace(owner->getID(), buildPrompt(owner)))
        waiting_result = true;
}

std::string LLMThinker::buildPrompt(const std::shared_ptr<Thing>& owner) const
{
    if (!prompt.empty())
        return prompt;

    return "<|im_start|>system\n"
           "You are " +
           owner->name +
           ", a strange NPC guide in a multiplayer text adventure. Output exactly one lowercase "
           "game command. Use either say <message> or go <direction>. Directions are left, right, "
           "up, down. Be brief, concrete, odd, and useful. Valid examples:\n"
           "say check the room before choosing a direction\n"
           "say someone moved that while nobody was looking\n"
           "go left\n"
           "go right\n"
           "say the quiet path is usually hiding something\n"
           "<|im_end|>\n"
           "<|im_start|>user\n"
           "Choose one action now.<|im_end|>\n"
           "<|im_start|>assistant\n";
}

void LLMThinker::executeCommand(const std::shared_ptr<Thing>& owner,
                                World& world, const std::string& command) const
{
    auto event = CommandParser::Parse(command);
    if (event.type == Event::Type::Move && owner->_physical && owner->physical()->current_room)
    {
        const auto room = owner->physical()->current_room;
        if (event.target == "left")
            owner->physical()->doMove(owner, world, room->x - 1, room->y);
        else if (event.target == "right")
            owner->physical()->doMove(owner, world, room->x + 1, room->y);
        else if (event.target == "up")
            owner->physical()->doMove(owner, world, room->x, room->y + 1);
        else if (event.target == "down")
            owner->physical()->doMove(owner, world, room->x, room->y - 1);
        return;
    }

    if (event.type != Event::Type::Say)
        return;

    std::string message = event.target + ' ' + event.object + ' ' + event.extra;
    boost::algorithm::trim(message);
    if (message.empty())
        return;

    Event chat_event(Event::Type::Chat, event.verb, event.target, event.object, event.extra,
                     message);
    owner->notifier()->doNotify(owner, chat_event);
}
