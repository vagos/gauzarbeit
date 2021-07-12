#include "Script/ScriptedThing.hpp"
#include "Server.hpp"
#include "World.hpp"

int main()
{
    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), 23);

    Server server(23, io_service, endpoint);
    World world;

    ScriptedThing::InitLua();

    server.acceptConnections();

    for (;;)
    {
       io_service.poll();
       world.doUpdate();
       server.doUpdate(world);
    }

    return 0;
}
