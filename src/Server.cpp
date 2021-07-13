#include <string>
#include <algorithm>

#include "Networked.hpp"
#include "Player/PlayerNetworked.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

void Server::acceptConnections()
{

    // new asio stuff
    
    acceptor.async_accept( 
            [this](boost::system::error_code error, tcp::socket socket)
            {
                if (!error)
                {
                    socket.non_blocking(true);

                    std::clog << "Player connected!\n";

                    onClientConnect(std::move(socket));
                }

                acceptConnections();
            });
}

void Server::onClientConnect(tcp::socket socket)
{
    auto newPlayer = std::make_shared<Player>();

    newPlayer -> networked() -> socket = std::make_unique<tcp::socket>(std::move(socket));
    
    clients.push_back(newPlayer); // add the player to clients
}

void Server::sendMessage(tcp::socket &socket, const std::string &msg)
{
    boost::system::error_code error;

    boost::asio::write(socket, boost::asio::buffer(msg.c_str(), msg.size()), error);

    if (error) std::clog << error.message() << '\n';
}

std::string Server::getMessage(tcp::socket &socket)
{

    size_t len;
    
    char data[100];
    
    boost::system::error_code error;
    
    len = socket.receive(boost::asio::buffer(data, 100), 0, error);

    if (!error) return std::string(data, len); 

    return "";
}

void Server::doUpdate(World& world)
{

    // CHANGE THIS

    for (auto c : clients)
    {
        std::clog << *c << '\n';
        world.addPlayer(c);

        c -> physical() -> doMove(c, 0, 0);
    }

    clients.erase(clients.begin(), clients.end());
    
}

void Server::updateClients(World& world)
{
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
