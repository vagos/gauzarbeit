#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <string>

#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

#include "Script/ScriptedThing.hpp"
//#include "TestItem.hpp"


void Server::acceptConnections(World &world)
{
        
    if (!socketSelector.wait()) return;

    if (!socketSelector.isReady(socketListener)) return;

    // Create a new Player object. 
    
    auto newPlayer = std::make_shared<Player>();
    
    if (socketListener.accept( *newPlayer -> networked -> socket ) == sf::Socket::Done)
    {
        std::clog << "Player connected!\n";    
    }

    socketSelector.add(*newPlayer -> networked -> socket);
    

    newPlayer -> networked -> addResponse("Welcome! Use login {name} to log on: "); 

    world.addPlayer(newPlayer);

    //Testing
    newPlayer -> physical -> doMove(newPlayer, 0, 0);
    
    //newPlayer -> physical -> gainItem( std::make_shared<TestItem>() );

    auto myChair = std::make_shared<ScriptedThing >("Chair");

    //newPlayer -> physical -> getRoom() -> listThings.push_back( myChair );
    newPlayer -> physical -> gainItem(myChair);
    newPlayer -> physical -> getRoom() -> listThings.push_back( std::make_shared<ScriptedThing>("Vase") );

}

void Server::doUpdate()
{
    
}

sf::SocketSelector Server::socketSelector;
