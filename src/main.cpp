#include <random>


#include "Server.hpp"
#include "World.hpp"

int main()
{
    std::srand(std::time(NULL));
    
    Server server(rand() % 9999 + 100);
    World world;


    while (true)
    {
        server.acceptConnections(world);
        server.doUpdate(world);

    }





    return 0;
}
