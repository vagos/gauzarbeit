#include "Server.hpp"
#include "World.hpp"
#include "extension/LLM.hpp"
#include "llama.h"
#include "script/js/ScriptedThing.hpp"
#include "script/lua/ScriptedThing.hpp"
#include "system/RoomSystem.hpp"
#include <atomic>
#include <csignal>
#include <cstdlib>
std::atomic<bool> keep_running{true};

void HandleSignal(int)
{
    keep_running = false;
}

int main(int argc, char* argv[])
{
    /* LMInit(); */
    /* LMInference(); */

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

    // Register systems
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
