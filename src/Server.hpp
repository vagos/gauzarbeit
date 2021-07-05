#ifndef SERVER_HPP
#define SERVER_HPP

#include "SFML/Network.hpp"
#include <iostream>
#include <memory>
#include <sstream>

#include "Room.hpp"
#include "Player/Player.hpp"

class Server
{


public:

    Server(int port)
    {
        if (socketListener.listen(port) != sf::Socket::Done) exit(1);

        std::clog << "Server started on port " << port << "\n";

        socketSelector.add(socketListener);

    }
    
    void doUpdate(World& world); // Change this
    static sf::SocketSelector& getSocketSelector() {return socketSelector;}

private:

    void acceptConnections(World& world);

    void updateClients(World& world);

    
private:
    sf::TcpListener socketListener;
    static sf::SocketSelector socketSelector;

    std::vector<std::shared_ptr<Thing>> clients;

};

#endif//SERVER_HPP
