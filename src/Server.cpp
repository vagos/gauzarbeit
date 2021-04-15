#include "Server.hpp"

#include "Player/Player.hpp"
#include "TestItem.hpp"
#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <string>

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


    world.addPlayer(newPlayer);

    //Testing
    
    // newPlayer -> physical -> gainItem( std::make_shared<TestItem>() );

}

void Server::sendMessage(const std::string &message)
{

    for (const auto& p : World::getPlayers())
    {
        p -> networked -> addResponse(message);
    }

}

void Server::sendMessage(const std::string& recipient, const std::string& message)
{
    for (auto& p: World::getPlayers())
    {
        if (p -> sName == recipient)
        {
            p -> networked -> addResponse(message);
            
            return;
        }

    }
}

void Server::doUpdate(World &world)
{
    for (auto& player : world.listPlayers) // Get request
        player -> networked -> doUpdate(player);    
    
    for (auto& player : world.listPlayers) // Handle request
        player -> networked -> doUpdate(player);
    
    for (auto& player : world.listPlayers) 
        player -> physical -> doUpdate(player);
    
    for (auto& player : world.listPlayers) // Send response
        player -> networked -> doUpdate(player);    

}

sf::SocketSelector Server::socketSelector;

