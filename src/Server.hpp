#ifndef SERVER_HPP
#define SERVER_HPP

#include "SFML/Network.hpp"
#include <iostream>
#include <memory>
#include <sstream>

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

    void doUpdate(); // Change this
    
    static void sendMessage(const std::string& message);
    static void sendMessage(const std::string& recipient, const std::string& message);

    static const sf::SocketSelector& getSocketSelector() {return socketSelector;}

//    const std::vector< std::shared_ptr<Thing> >& getClients() {return clients;};


private:
    sf::TcpListener socketListener;
    static sf::SocketSelector socketSelector;

//    std::vector< std::shared_ptr<Thing> > clients;

};

#endif//SERVER_HPP
