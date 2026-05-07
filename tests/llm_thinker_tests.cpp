#include "Room.hpp"
#include "TestSupport.hpp"
#include "World.hpp"
#include "system/LLMSystem.hpp"
#include "thing/LLMNotifier.hpp"
#include <chrono>
#include <doctest/doctest.h>
#include <thread>

TEST_CASE("LLM notifier responds to chat from players")
{
    World world;

    LLMConfig config;
    config.model_path = "";
    LLMSystem llm(config, [](const std::string&) { return std::string("Roger that."); });

    auto room = std::make_shared<Room>(7, 7);

    auto player = MakeBasicThing("Player");
    player->is_player = true;
    player->physical()->current_room = room;
    room->addThing(player);

    auto npc = MakeBasicThing("Guide");
    npc->_notifier = std::make_shared<LLMNotifier>(0, 8, &llm);
    npc->physical()->current_room = room;
    room->addThing(npc);

    player->notifier()->event.payload = "hello there";
    npc->notifier()->onNotify(npc, player, Event::Type::Chat, nullptr);

    auto player_net = std::dynamic_pointer_cast<TestNetworked>(player->networked());
    REQUIRE(player_net != nullptr);

    for (int i = 0; i < 50; ++i)
    {
        llm.doUpdate(world);
        npc->notifier()->doUpdate(npc);
        if (player_net->response().find("> Guide: Roger that.") != std::string::npos)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    CHECK(player_net->response().find("> Guide: Roger that.") != std::string::npos);
}

TEST_CASE("LLM notifier responds to targeted ask")
{
    World world;

    LLMConfig config;
    config.model_path = "";
    LLMSystem llm(config, [](const std::string&) { return std::string("I can help with tasks."); });

    auto room = std::make_shared<Room>(8, 8);

    auto player = MakeBasicThing("Player");
    player->is_player = true;
    player->physical()->current_room = room;
    room->addThing(player);

    auto npc = MakeBasicThing("Guide");
    npc->_notifier = std::make_shared<LLMNotifier>(0, 8, &llm);
    npc->physical()->current_room = room;
    room->addThing(npc);

    player->notifier()->event.object = "kitchen";
    player->notifier()->event.extra = "tasks";
    npc->notifier()->onNotify(npc, player, Event::Type::Ask, npc);

    auto player_net = std::dynamic_pointer_cast<TestNetworked>(player->networked());
    REQUIRE(player_net != nullptr);

    for (int i = 0; i < 50; ++i)
    {
        llm.doUpdate(world);
        npc->notifier()->doUpdate(npc);
        if (player_net->response().find("> Guide: I can help with tasks.") != std::string::npos)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    CHECK(player_net->response().find("> Guide: I can help with tasks.") != std::string::npos);
}
