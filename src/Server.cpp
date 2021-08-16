#include <string>
#include <algorithm>

#include "thing/Networked.hpp"
#include "player/PlayerNetworked.hpp"
#include "Server.hpp"
#include "World.hpp"
#include "player/Player.hpp"

#include "script/LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"

#include "Exceptions.hpp"

Server::Server(int port, boost::asio::io_service &io_service, const tcp::endpoint &endpoint): 
    acceptor(io_service, endpoint), s_socket(io_service) 
{
    std::clog << "Server started on port " << port << "\n";
    acceptor.non_blocking(true);
}


void Server::acceptConnections()
{
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

    if (error == boost::asio::error::eof)
    {
        throw PlayerDisconnect();
    }

    return "";
}

void Server::doUpdate(World& world)
{
    acceptConnections();
    updateClients(world);
}

void Server::updateClients(World& world)
{
    for (const auto& c : clients)
    {
        c -> networked() -> sendResponse(c);
        c -> networked() -> getRequest(c, world);
        c -> networked() -> handleRequest(c, world);
    }
    
    for (const auto& c : clients)
    {
        if ( c->networked()->isOnline()) 
        {
            world.addPlayer(c);
            c->physical()->doMove(c, 0, 0);
        }
    }
}

std::string Server::MOTD;
