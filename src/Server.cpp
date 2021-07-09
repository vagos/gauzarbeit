#include <SFML/Network/SocketSelector.hpp>
#include <string>
#include <algorithm>

#include "Networked.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

#include "Script/ScriptedThing.hpp"
//#include "Character/Character.hpp"
#include "Character/BasicEnemy.hpp"


void Server::acceptConnections(World &world)
{
    if (!socketSelector.isReady(socketListener)) return;

    // Create a new Player object. 
    
    auto newPlayer = std::make_shared<Player>();
    
    if (socketListener.accept( *newPlayer -> networked() -> socket ) == sf::Socket::Done)
    {
        std::clog << "Player connected!\n";    
    }

    socketSelector.add(*newPlayer -> networked() -> socket);

    newPlayer -> networked() -> addResponse("Welcome! Use login {name} to log on.\n"); 

    //clients.push_back(newPlayer);

    world.addPlayer(newPlayer);

    //Testing
    ScriptedThing::InitLua();
    
    newPlayer -> physical() -> doMove(newPlayer, 0, 0);
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
        c -> networked() -> getRequest(c, world);
    for (const auto& c : clients)
        c -> notifier() -> setEvent(c);
    for (const auto& c : clients)
        c -> networked() -> handleRequest(c, world);
    for (const auto& c : clients)
        c -> notifier() -> clearEvent();
    for (const auto& c : clients)
        c -> networked() -> sendResponse(c);
    
    for (const auto& c : clients)
    {
        if (c -> networked() -> isOnline())
        {
            world.addPlayer(c);
        }
    }

    clients.erase( std::remove_if(clients.begin(), clients.end(), 
                [](const std::shared_ptr<Thing>& p) 
                { return p -> networked() -> isOnline();}), clients.end());
}

sf::SocketSelector Server::socketSelector;
