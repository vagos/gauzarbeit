#include "Server.hpp"
#include "Exceptions.hpp"
#include "World.hpp"
#include "player/Player.hpp"
#include "player/PlayerNetworked.hpp"
#include "script/LuaHelpers.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Networked.hpp"
#include <algorithm>
#include <string>
#include <thread>

Server::Server(int port, boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
    : acceptor(io_service, endpoint), s_socket(io_service)
{
    Log("Server started on port " << port);
    acceptor.non_blocking(true);
}

void Server::acceptConnections()
{
    boost::system::error_code error;

    tcp::socket socket = acceptor.accept(error);

    if (error)
        return;

    onClientConnect(std::move(socket));
}

void Server::onClientConnect(tcp::socket socket)
{
    Log("A player connected!");

    socket.non_blocking(true);

    auto newPlayer = std::make_shared<Player>();

    newPlayer->networked()->socket = std::make_unique<tcp::socket>(std::move(socket));
    newPlayer->networked()->addResponse(MOTD);

    clients.push_back(newPlayer); // add the player to clients
}

void Server::sendMessage(tcp::socket& socket, const std::string& msg)
{
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg.c_str(), msg.size()), error);

    if (error)
        Log("Cannot send message: " << error.message());
}

std::string Server::getMessage(tcp::socket& socket)
{

    size_t len;
    char data[100];

    boost::system::error_code error;
    len = socket.receive(boost::asio::buffer(data, 100), 0, error);
    if (!error)
        return std::string(data, len);

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
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // TODO: Avoid this
}

void Server::updateClients(World& world)
{
    for (const auto& c : clients)
    {
        c->networked()->sendResponse(c);
        c->networked()->getRequest(c, world);
        c->networked()->handleRequest(c, world);
    }

    for (const auto& c : clients)
    {
        if (c->networked()->isOnline() && !c->physical()->current_room)
        {
            world.addPlayer(c);
            c->physical()->doMove(c, 0, 0); // TODO: Move this
        }
    }

    // close the sockets of all !isOnline client
    for (const auto& c : clients)
    {
        if (!c->networked()->isOnline())
        {
            c->networked()->socket->close();
        }
    }
}

std::string Server::MOTD;
