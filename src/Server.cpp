#include <string>
#include <algorithm>

#include "Networked.hpp"
#include "Player/PlayerNetworked.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "Player/Player.hpp"

#include "Script/LuaHelpers.hpp"
#include "Script/ScriptedThing.hpp"

Server::Server(int port, boost::asio::io_service &io_service, const tcp::endpoint &endpoint): 
    acceptor(io_service, endpoint), s_socket(io_service) 
{
    std::clog << "Server started on port " << port << "\n";

    acceptor.non_blocking(true);

}


void Server::acceptConnections()
{

    // new asio stuff

    boost::system::error_code error;
    
    tcp::socket socket = acceptor.accept(error);
    
    if (error) return;

    onClientConnect(std::move(socket));
    
}

void Server::onClientConnect(tcp::socket socket)
{
    std::clog << "Player connected!\n";
    
    socket.non_blocking(true);

    auto newPlayer = std::make_shared<Player>();

    newPlayer -> networked() -> socket = std::make_unique<tcp::socket>(std::move(socket));

    newPlayer -> networked() -> addResponse( MOTD );
    
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

std::string Server::MOTD;
