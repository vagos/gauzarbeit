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

    PlayerNetworked(): state(State::Entering)
    {
    }

    void doUpdate( std::shared_ptr<Thing> owner ) override // Freaking fix this.
    {
        switch (nState)
        {
            case 0:
                clearStreams();
                getRequest(owner);
                nState = 1;
                break;

            case 1:

                handleRequest(owner);
                nState = 2;
                break;

            case 2:

                sendResponse();
                nState = 0;
                break;


        }
    }
    

     
private: 
    
    void getRequest( std::shared_ptr<Thing> owner )
    {
        if ( ! Server::getSocketSelector().isReady( *socket ) ) return;

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
    void handleRequest(std::shared_ptr<Thing> owner)
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
    void sendResponse( )
    {
        if (! streamResponse.str().size() ) return;

        std::clog << "Gonna send response: " << streamResponse.str() << "\n";
        socket -> send(streamResponse.str().c_str(), streamResponse.str().size());
    }

    std::size_t playerId;
    
    State state;
    int nState = 0;

    // last online
    // ip
};

#endif
