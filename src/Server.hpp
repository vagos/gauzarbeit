#ifndef SERVER_HPP
#define SERVER_HPP

#include "SFML/Network.hpp"
#include <iostream>
#include <memory>
#include <sstream>

#include "World.hpp"
#include "Player.hpp"
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

    void acceptConnections(World& world)
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

    void doUpdate(World& world) // Change this
    {
        for (auto& player : world.listPlayers)
        {
            player -> networked -> doUpdate(socketSelector);    

            //player -> physical -> doUpdate();

            static_cast<PlayerNetworked*>(player -> networked.get()) -> sendResponse(socketSelector);
        }
    }

private:
    sf::TcpListener socketListener;
    sf::SocketSelector socketSelector;

};

#endif//SERVER_HPP
