#include "ScriptedThing_JS.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "ex/LLM.hpp"
#include "llama.h"
#include "script/ScriptedThing.hpp"
#include "system/RoomSystem.hpp"
#include <cstdlib>
#include <quickjs.h>

int main(int argc, char* argv[])
{
    /* LMInit(); */
    /* LMInference(); */

    ScriptedThing::InitLua();
    ScriptedThing_JS::Init();
    auto grape = std::make_shared<ScriptedThing_JS>("Grape");
    ScriptedThing_JS::DeInit();

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
