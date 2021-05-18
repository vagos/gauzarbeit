#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <string>

#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

#include "Script/ScriptedThing.hpp"
#include "Character/Character.hpp"


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
    

    newPlayer -> networked -> addResponse("Welcome! Use login {name} to log on.\n"); 

    world.addPlayer(newPlayer);

    //Testing
    ScriptedThing::InitLua();
    
    newPlayer -> physical -> doMove(newPlayer, 0, 0);
    
    auto myChair = std::make_shared<ScriptedThing>("Chair");
    auto myWinston = std::make_shared<ScriptedThing>("Winston");
    auto myGoblin = std::make_shared<ScriptedThing>("Goblin");
    auto myBokeBall = std::make_shared<ScriptedThing>("BokeBall");

    auto myShopKeeper = std::make_shared<Character>("Shop Man");

    newPlayer -> physical -> gainItem(myChair);
    newPlayer -> physical -> gainItem(myWinston);
    newPlayer -> physical -> gainItem(myBokeBall);

    newPlayer -> physical -> getRoom() -> addThing(myGoblin);
    newPlayer -> physical -> getRoom() -> addThing(myShopKeeper);
}

void Server::doUpdate(World& world)
{
    acceptConnections(world);
}

sf::SocketSelector Server::socketSelector;
