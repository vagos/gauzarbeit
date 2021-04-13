#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Thing.hpp"
#include "Room.hpp"
#include <SFML/Network/SocketSelector.hpp>
#include <bits/c++config.h>
#include <memory>
#include <sstream>


class PlayerNetworked : public Networked 
{

public:

    PlayerNetworked(std::shared_ptr<Thing> owner): Networked(owner), state(State::Entering)
    {
    }

    void doUpdate( sf::SocketSelector& socketSelector ) override
    {

        streamRequest.str(std::string()); // Clear the string stream.
        streamResponse.str(std::string()); 

        getRequest(socketSelector);
        
    }
    
    const std::stringstream& getRequestStream() { return streamRequest; }
    std::stringstream& getResponseStream() { return streamResponse; }


private: 
    
    void getRequest( sf::SocketSelector& socketSelector )
    {
        if ( ! socketSelector.isReady( *socket ) ) return;

        if (socket->receive(cData, 100, nReceived) != sf::Socket::Done)
        {
            std::clog << "Socket error!\n";
            return;
        }

        if (  cData[nReceived - 1] != '\n' ) return;
        
        streamRequest << std::string{cData, nReceived - 2};

        std::clog << *owner << ": " << streamRequest.str() << " Received: " << nReceived << "\n";

    }
public:
    void sendResponse( sf::SocketSelector& socketSelector )
    {
        if (! streamResponse.str().size() ) return;

        std::clog << "Gonna send response: " << streamResponse.str() << "\n";
        socket -> send(streamResponse.str().c_str(), streamResponse.str().size());
    }

        enum class State
    {
        Entering,
        EnterPassword,
        LoggedIn,
    };

    std::size_t playerId;
    
    bool bLoggenIn = false;
    char cData[100];
    std::size_t nReceived;

    std::stringstream streamRequest;
    std::stringstream streamResponse;

    State state;

    
    // last online
    // ip
};

class PlayerPhysical : public Physical
{
public:

   PlayerPhysical(std::shared_ptr<Thing> owner): Physical(owner) 
    {
        currentRoom = Room::get(0, 0); 
    }

   void doUpdate() override
   {
        
        PlayerNetworked * net = static_cast<PlayerNetworked *>(owner -> networked.get()); 
        
        std::stringstream requestStream { net->getRequestStream().str() };

        std::string sVerb; 

        requestStream >> sVerb;

        if (sVerb == "move")
        {
            std::string sDirection;

            requestStream >> sDirection;

            moveDirection(sDirection);

        }
        


   }

private:

   void moveDirection(const std::string& direction)
   {
        if (direction == "left")
        {
            doMove(currentRoom -> x - 1, currentRoom -> y);
        }
        
        else if (direction == "right")
        {
            doMove(currentRoom -> x + 1, currentRoom -> y);
        }
   
        else if (direction == "down")
        {
            doMove(currentRoom -> x, currentRoom -> y + 1);
        }
   
        else if (direction == "up")
        {
            doMove(currentRoom -> x, currentRoom -> y - 1);
        }

        else return;

        static_cast<PlayerNetworked *>(owner -> networked.get() ) 
            -> getResponseStream() << "Moved into Room " << currentRoom -> x << " " << currentRoom -> y << "\n"; 


        
   }

};

class Player : public Thing
{
public:
    Player() 
    {

    }


    void doInit()
    {
       networked = std::make_unique<PlayerNetworked>(shared_from_this());
       physical = std::make_unique<PlayerPhysical>(shared_from_this());
    }


};

#endif//PLAYER_HPP
