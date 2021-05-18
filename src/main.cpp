#include <random>

#include "Script/ScriptedThing.hpp"
#include "Server.hpp"
#include "World.hpp"


int main()
{
    std::srand(std::time(NULL));
    
    Server server(rand() % 9999 + 1000);
    //Server server(1234);
    World world;

    while (true)
    {
       world.doUpdate();
       server.doUpdate(world);
    }

    return 0;
}
