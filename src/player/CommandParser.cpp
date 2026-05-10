#include "player/CommandParser.hpp"
#include "Exceptions.hpp"
#include "player/Player.hpp"
#include <boost/algorithm/string/case_conv.hpp>
#include <sstream>

Event CommandParser::Parse(const std::string& request)
{
    Event event{};

    std::stringstream req{request};
    req >> event.verb >> event.target >> event.object;
    std::getline(req >> std::ws, event.extra);

    boost::to_lower(event.verb);

    if (event.verb.empty())
    {
        event.type = Event::Type::Invalid;
        return event;
    }

    const auto it = Player::playerCommands.find(event.verb);
    if (it == Player::playerCommands.end())
        throw InvalidCommand();

    event.type = it->second;

    return event;
}
