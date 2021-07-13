#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <iostream>
#include <memory>
#include <sstream>

#include <boost/asio.hpp>

#include "Room.hpp"
#include "Player/Player.hpp"

using boost::asio::ip::tcp;

class Server
{


public:

    Server(int port, boost::asio::io_service &io_service, const tcp::endpoint &endpoint): 
        acceptor(io_service, endpoint), s_socket(io_service) 
    {
        std::clog << "Server started on port " << port << "\n";
    }
    
    void doUpdate(World& world); // Change this

    static std::string getMessage( tcp::socket& socket );
    static void sendMessage( tcp::socket& socket, const std::string &msg );

    void acceptConnections();

    void onClientConnect(tcp::socket socket);

private:
    void updateClients(World& world);

    
private:
    std::vector<std::shared_ptr<Thing>> clients;

    tcp::acceptor acceptor;
    tcp::socket s_socket;
};

#endif//SERVER_HPP
