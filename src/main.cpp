#include "Script/ScriptedThing.hpp"
#include "Server.hpp"
#include "World.hpp"

#include <cstdlib>
#include <random>

int main()
{
    //srand(time(NULL));
    //int ip = random() % 9999 + 100;
    
    ScriptedThing::InitLua();

    int ip = 23;

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), ip);

    Server server(ip, io_service, endpoint);
    World world;


    for (;;)
    {
       server.acceptConnections();
       world.doUpdate();
       server.doUpdate(world);
    }

    return 0;
}
