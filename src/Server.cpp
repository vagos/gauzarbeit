#include "Server.hpp"

#include "Player/Player.hpp"
#include <string>

void Server::acceptConnections(World &world)
{
        
    if (!socketSelector.wait()) return;

    if (!socketSelector.isReady(socketListener)) return;

    // Create a new Player object. 
    
    auto newPlayer = std::make_shared<Player>();
    newPlayer -> doInit();
    
    if (socketListener.accept( *newPlayer -> networked -> socket ) == sf::Socket::Done)
    {
        std::clog << "Player connected!\n";    
    }

    socketSelector.add(*newPlayer -> networked -> socket);

    world.addPlayer(newPlayer);


}

void Server::sendMessage(const std::string &message)
{

    for (const auto& p : World::getPlayers())
    {
        std::static_pointer_cast<PlayerNetworked>(p -> networked)
            -> addResponse(message);
    }

}

void Server::sendMessage(const std::string& recipient, const std::string& message)
{
    for (auto& p: World::getPlayers())
    {
        if (p -> sName == recipient)
        {
            std::static_pointer_cast<PlayerNetworked>(p -> networked)
                -> addResponse(message);
            
            return;
        }

    }
}

void Server::doUpdate(World &world)
{
    for (auto& player : world.listPlayers)
        player -> networked -> doUpdate(socketSelector);    

    for (auto& player : world.listPlayers) 
        static_cast<PlayerNetworked*>(player -> networked.get())
            -> handleRequest();

    for (auto& player : world.listPlayers) 
        player -> physical -> doUpdate();

    for (auto& player : world.listPlayers) 
        static_cast<PlayerNetworked*>(player -> networked.get()) 
            -> sendResponse(socketSelector);
}
