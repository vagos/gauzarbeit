#include <random>

#include "Server.hpp"
#include "World.hpp"


int main()
{
    std::srand(std::time(NULL));
    
    Server server(rand() % 9999 + 100);
    //Server server(1234);
    World world;

    
    while (true)
    {
      // server.doUpdate();

       world.doUpdate();
       
       server.acceptConnections(world);



    }

    return 0;
}
