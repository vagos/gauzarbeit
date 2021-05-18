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
            
            setState( State::LoggedOut ); 

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

        auto& event = owner -> notifier -> event;

        if ( event.verb == "login" )
        {
            if ( state != State::Entering)
            {
                addResponse("You are already loggen in!\n", Color::Red);
                return;
            }

            owner -> sName = owner -> notifier -> event.noun;

            setState( State::LoggedIn ); 

            addResponse("You are logged in as " + owner -> sName + "\n");

        }

        else if ( event.verb == "msg" )
        {
            addMessage(event.extra);

        }

        else if ( event.verb == "whisper" )
        {
            //addMessage(event.extra, event.noun);

            owner -> notifier -> doNotify(owner, Notifier::Event::Type::Whisper);

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
