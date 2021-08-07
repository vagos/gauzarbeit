#include "Script/ScriptedThing.hpp"
#include "Server.hpp"
#include "World.hpp"

#include <cstdlib>
#include <random>
#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>


#include "unistd.h"

int main()
{
    ScriptedThing::InitLua();

    int ip = 23;

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), ip);

    Server server(ip, io_service, endpoint);
    World world;

/*    if ( std::freopen( "Log.txt", "w+", stderr ) )
    {
        std::cout << "Logging!\n";
    }
*/

    for (;;)
    {
       std::this_thread::sleep_for( std::chrono::seconds(1) ); 

       server.doUpdate(world);
       world.doUpdate();
    }

    return 0;
}
