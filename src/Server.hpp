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
            std::clog << "New dude connected!\n";    
        }

        socketSelector.add(*newPlayer -> networked -> socket);

        world.addPlayer(newPlayer);

    }

    void doUpdate(World& world)
    {
        for (auto& player : world.listPlayers)
        {
            player -> networked -> doUpdate(socketSelector);    
        }
    }


    void handleRequest(std::shared_ptr<Thing>& player, std::stringstream& request)
    {
        
        std::cout << request.str() << "\n"; 

        std::string sVerb;

        request >> sVerb;

        std::stringstream response{""};

        if (sVerb == "clear" || sVerb == "cls")
        {
            response << "\e[2J\e[H";

        }

        if (sVerb == "help")
        {
            response << "1. Type 'cls' to clear the screen\n";
            response << "2. Type 'dab' to dab on 'em.\n";
        }

        if (sVerb == "dab")
        {
            response << "You dabbed on 'em\n";
        }

        if (sVerb == "move")
        {
            int x, y;
            
            request >> x >> y;

            player -> physical -> doMove(x,y);

            response << "You got moved into room "
                << "x: " << player -> physical -> getRoom() -> x
                << "y: " << player -> physical -> getRoom() -> y
                << "\n";
        }

        if (sVerb == "exit")
        {
            
            delete player.get();
        
        }

        if (response.str().size())
        {
            player -> networked -> socket -> send(response.str().data(), response.str().size());
        }

    }

private:
    sf::TcpListener socketListener;
    sf::SocketSelector socketSelector;

};

#endif//SERVER_HPP
