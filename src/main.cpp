#include <random>

#include "Script/ScriptedThing.hpp"
#include "Server.hpp"
#include "World.hpp"

int main()
{
    std::srand(std::time(NULL));

    int port = rand() % 9999 + 1000;
    
    //Server server(port);
    Server server(23);
    World world;

    FILE * f = fopen("port.txt", "w+");

    fprintf(f, "%d", port);

    fclose(f);

    while (true)
    {
       world.doUpdate();
       server.doUpdate(world);
    }

    return 0;
}
