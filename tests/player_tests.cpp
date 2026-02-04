#include "Exceptions.hpp"
#include "player/CommandParser.hpp"
#include "player/Player.hpp"
#include <doctest/doctest.h>

TEST_CASE("PlayerNotifier parses commands into events")
{
    Player::setPlayerCommands();
    auto event = CommandParser::Parse("Say Hello there friend");

    CHECK_EQ(event.verb, "say");
    CHECK_EQ(event.target, "Hello");
    CHECK_EQ(event.object, "there");
    CHECK_EQ(event.extra, "friend");
    CHECK_EQ(event.type, Event::Type::Say);
}

TEST_CASE("PlayerNotifier rejects unknown commands")
{
    Player::setPlayerCommands();
    CHECK_THROWS_AS(CommandParser::Parse("nonsense"), InvalidCommand);
}

TEST_CASE("PlayerNotifier handles movement targets")
{
    Player::setPlayerCommands();
    auto event = CommandParser::Parse("move left");

    CHECK_EQ(event.verb, "move");
    CHECK_EQ(event.target, "left");
    CHECK_EQ(event.type, Event::Type::Move);
}
