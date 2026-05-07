#include "Server.hpp"
#include "Helpers.hpp"
#include "World.hpp"
#include "script/js/ScriptedThing.hpp"
#include "script/lua/ScriptedThing.hpp"
#include "system/LLMSystem.hpp"
#include "system/RoomSystem.hpp"
#include "system/WorldGenSystem.hpp"
#include "thing/LLMNotifier.hpp"
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <string>

std::atomic<bool> keep_running{true};

void HandleSignal(int)
{
    keep_running = false;
}

int main(int argc, char* argv[])
{
    ScriptedThing_Lua::Init();
    ScriptedThing_JS::Init();

    int port = 23;
    if (argc >= 2)
    {
        port = std::atoi(argv[1]);
    }

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), port);

    Server server(port, io_service, endpoint);
    World world;
    world.systems.push_back(std::make_unique<WorldGenSystem>());

    try
    {
        auto llm_system = std::make_unique<LLMSystem>();
        auto guide_bot = std::make_shared<Thing>("GuideBot");
        guide_bot->_networked = std::make_shared<Networked>();
        guide_bot->_physical = std::make_shared<Physical>();
        guide_bot->_attackable = std::make_shared<Attackable>();
        guide_bot->_notifier = std::make_shared<LLMNotifier>(10, 8, llm_system.get());
        guide_bot->_achiever = std::make_shared<Achiever>();
        guide_bot->_tasker = std::make_shared<Tasker>();
        guide_bot->_inspectable = std::make_shared<Inspectable>();
        guide_bot->_talker = std::make_shared<Talker>();
        guide_bot->physical()->doMove(guide_bot, 0, 0);
        world.systems.push_back(std::move(llm_system));
    }
    catch (const std::exception& e)
    {
        Log("LLM disabled: " << e.what());
    }
    world.systems.push_back(std::make_unique<RoomSystem>());

    std::signal(SIGINT, HandleSignal);
    std::signal(SIGTERM, HandleSignal);

    while (keep_running)
    {
        server.doUpdate(world);
        world.doUpdate();
    }

    // Persist rooms to database on shutdown
    for (auto& [key, room] : Room::mapRooms)
    {
        (void)key;
        if (room && room->_networked)
            room->networked()->doDatabaseStore(room);
    }

    return 0;
}
