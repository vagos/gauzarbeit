#ifndef SERVER_HPP
#define SERVER_HPP

#include "SFML/Network.hpp"
#include <iostream>
#include <memory>
#include <sstream>

#include "World.hpp"
#include "Room.hpp"

class Server
{


public:

    Server(int port)
    {
        if (socketListener.listen(port) != sf::Socket::Done) exit(1);

        std::clog << "Server started on port " << port << "\n";

        socketSelector.add(socketListener);
    }

    void acceptConnections(World& world);

    void doUpdate(World& world); // Change this
    
    static void sendMessage(const std::string& message);
    static void sendMessage(const std::string& recipient, const std::string& message);

    static const sf::SocketSelector& getSocketSelector() {return socketSelector;}


private:
    sf::TcpListener socketListener;
    static sf::SocketSelector socketSelector;

};

#endif//SERVER_HPP
