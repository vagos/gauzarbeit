#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <iostream>
#include <lua.h>
#include <memory>
#include <sstream>

#include <boost/asio.hpp>

#include "Room.hpp"
#include "player/Player.hpp"


using boost::asio::ip::tcp;

class Server
{


public:

    Server(int port, boost::asio::io_service &io_service, const tcp::endpoint &endpoint);
    
    void doUpdate(World& world); // Change this

    static std::string getMessage( tcp::socket& socket );
    static void sendMessage( tcp::socket& socket, const std::string &msg );

    void acceptConnections();

    void onClientConnect(tcp::socket socket);
    
    static std::string MOTD;

private:
    void updateClients(World& world);
    
    std::vector<std::shared_ptr<Thing>> clients;

    tcp::acceptor acceptor;
    tcp::socket s_socket;

};

#endif//SERVER_HPP
