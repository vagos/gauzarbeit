#include <SFML/Network/SocketSelector.hpp>
#include <string>
#include <algorithm>

#include "Networked.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

#include "Script/ScriptedThing.hpp"
#include "Character/Character.hpp"
#include "Character/BasicEnemy.hpp"


void Server::acceptConnections(World &world)
{
    
    if (!socketSelector.isReady(socketListener)) return;

    // Create a new Player object. 
    
    auto newPlayer = std::make_shared<Player>();
    
    if (socketListener.accept( *newPlayer -> networked -> socket ) == sf::Socket::Done)
    {
        std::clog << "Player connected!\n";    
    }

    socketSelector.add(*newPlayer -> networked -> socket);

    newPlayer -> networked -> addResponse("Welcome! Use login {name} to log on.\n"); 

    //clients.push_back(newPlayer);

    world.addPlayer(newPlayer);

    //Testing
    ScriptedThing::InitLua();
    
    newPlayer -> physical -> doMove(newPlayer, 0, 0);
    
    auto myChair = std::make_shared<ScriptedThing>("Chair");
    auto myWinston = std::make_shared<ScriptedThing>("Winston");
    auto myGoblin = std::make_shared<ScriptedThing>("Goblin");
    auto myBokeBall = std::make_shared<ScriptedThing>("BokeBall");
    auto myBall = std::make_shared<ScriptedThing>("Ball");

    auto myBigRat = std::make_shared<BasicEnemy>("BigRat");
    auto myShopKeeper = std::make_shared<Character>("ShopMan");

    newPlayer -> physical -> gainItem(myChair);
    newPlayer -> physical -> gainItem(myWinston);
    newPlayer -> physical -> gainItem(myBokeBall);
    newPlayer -> physical -> gainItem(myBall);

    newPlayer -> achiever -> addQuest( std::make_shared<ScriptedQuest>("Ratz") );

    myGoblin -> physical -> doMove(myGoblin, newPlayer -> physical -> current_room -> x, newPlayer -> physical -> current_room -> y);
    myShopKeeper -> physical -> doMove(myShopKeeper, newPlayer -> physical -> current_room -> x, newPlayer -> physical -> current_room -> y);
    myBigRat -> physical -> doMove(myBigRat, newPlayer -> physical -> current_room -> x, newPlayer -> physical -> current_room -> y);

}

void Server::doUpdate(World& world)
{
    if (!socketSelector.wait(sf::milliseconds(100))) return;

    acceptConnections(world);
    //updateClients(world);
}

void Server::updateClients(World& world)
{
    //if (!socketSelector.wait(sf::milliseconds(100))) return;

    for (const auto& c : clients)
        c -> networked -> getRequest(c, world);
    for (const auto& c : clients)
        c -> notifier -> setEvent(c);
    for (const auto& c : clients)
        c -> networked -> handleRequest(c, world);
    for (const auto& c : clients)
        c -> notifier -> clearEvent();
    for (const auto& c : clients)
        c -> networked -> sendResponse(c);
    
    for (const auto& c : clients)
    {
        if (c -> networked -> isOnline())
        {
            world.addPlayer(c);
        }
    }

    clients.erase( std::remove_if(clients.begin(), clients.end(), 
                [](const std::shared_ptr<Thing>& p) 
                { return p -> networked -> isOnline();}), clients.end());
}

sf::SocketSelector Server::socketSelector;
