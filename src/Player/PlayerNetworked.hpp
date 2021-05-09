#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <sstream>
#include <string>

#include "../Thing.hpp"
#include "../Server.hpp"

class World;

class PlayerNetworked : public Networked 
{

public:

    PlayerNetworked(): state(State::Entering)
    {
        //state = State::LoggedIn;
    }

    void getRequest( std::shared_ptr<Thing> owner, World& world )
    {
        if ( ! Server::getSocketSelector().isReady( *socket ) ) return;

        if (socket->receive(cData, 100, nReceived) != sf::Socket::Done)
        {
            std::clog << "Socket error!\n"; 
            
            // world.removePlayer(owner);

            setState( State::LoggedOut ); // Remove player in a seperate loop.

            return;
        }

        if (  cData[nReceived - 1] != '\n' ) return;
        
        streamRequest << std::string{cData, nReceived - 2};

        std::clog << *owner << ": " << streamRequest.str() << " Received: " << nReceived << " bytes" << "\n";

    }
    
    void sendResponse(std::shared_ptr<Thing> owner) override
    {
        //if ( ! Server::getSocketSelector().isReady( *socket ) ) return;
        if ( ! streamResponse.str().size() ) goto CLEAR;

        // std::clog << "Gonna send response: " << streamResponse.str() << "\n";
        socket -> send(streamResponse.str().c_str(), streamResponse.str().size());
        
        CLEAR:
            
        clearStreams();
    }
    
    void handleRequest(std::shared_ptr<Thing> owner, World& world) override
    {
        std::stringstream req { getRequestStream().str() };
        
        std::string sVerb;

        req >> sVerb;

        if ( sVerb == "login" )
        {
            if ( state != State::Entering)
            {
                addResponse("You are already loggen in!\n", Color::Red);
                return;
            }

            req >> owner -> sName;

            setState( State::LoggedIn ); 

            addResponse("You are logged in as " + owner -> sName + "\n");

        }

        else if ( sVerb == "msg" )
        {
            std::string message;

            req >> message;

            addMessage(message);

        }

        else if ( sVerb == "whisper" )
        {
            std::string recipient, message;

            req >> recipient >> message;

            addMessage(message, recipient);

        }

    }

    bool isOnline() {return state == State::LoggedIn;}

    bool isOffline() {return state == State::LoggedOut;}

private: 
    enum class State
    {
        Entering,
        LoggedIn,
        LoggedOut,
    };


    void setState(State s) {state = s;}

    

public:
    State state;
    // last online
    // ip
};

#endif
