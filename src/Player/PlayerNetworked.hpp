#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "../Thing.hpp"
#include "../World.hpp"
#include "../Server.hpp"

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

    void addResponse(const std::string& res)
    {
       streamResponse << res; 
    }


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

        std::clog << *owner << ": " << streamRequest.str() << " Received: " << nReceived << " bytes" << "\n";

    }

public:
    void handleRequest()
    {
        std::stringstream req { getRequestStream().str() };
        
        std::string sVerb;

        req >> sVerb;

        if (sVerb == "" && state == State::Entering)
        {
            addResponse("Welcome! Use login {name} to log on: ");
            return;
        }
        
        if ( sVerb == "login" )
        {
            if ( state != State::Entering)
            {
                addResponse("You are already loggen in!\n");
                return;
            }

            req >> owner -> sName;

            setState( State::LoggedIn ); 

            addResponse("You are logged in as " + owner -> sName + "\n");

            return;
        }

        if ( sVerb == "msg" )
        {
            std::string message;

            req >> message;

            Server::sendMessage(owner -> sName + " " + message + "\n");

            return;

        }

        if ( sVerb == "whisper" )
        {
            std::string recipient, message;

            req >> recipient >> message;

            Server::sendMessage(recipient, owner->sName + " whispers to you: " + message);

        }

    }
    
    enum class State
    {
        Entering,
        LoggedIn,
    };


    void setState(State s) {state = s;}

public:
    void sendResponse( sf::SocketSelector& socketSelector )
    {
        if (! streamResponse.str().size() ) return;

        std::clog << "Gonna send response: " << streamResponse.str() << "\n";
        socket -> send(streamResponse.str().c_str(), streamResponse.str().size());
    }

   
    std::size_t playerId;
    
    char cData[100];
    std::size_t nReceived;

    std::stringstream streamRequest;
    std::stringstream streamResponse;

    State state;

    // last online
    // ip
};

#endif
