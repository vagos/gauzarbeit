#include "Server.hpp"
#include "World.hpp"
#include "script/ScriptedThing.hpp"
#include "system/RoomSystem.hpp"
#include "unistd.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>

int main(int argc, char* argv[])
{
    ScriptedThing::InitLua();

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

    for (;;)
    {
        server.doUpdate(world);
        world.doUpdate();
    }

    return 0;
}
