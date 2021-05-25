#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>

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
            std::clog << "User disconnected!\n"; 
            
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
                addResponse( ColorString("You are already loggen in!\n", Color::Red) );
                return;
            }

            owner -> name = owner -> notifier -> event.noun;

            setState( State::LoggedIn ); 

            addResponse( ColorString("You are logged in as " + owner -> name + ".\n", Color::Green) );

        }

        else if ( event.verb == "msg" )
        {
            owner -> notifier -> doNotify(owner, Notifier::Event::Type::Chat); 
        }

        else if ( event.verb == "whisper" )
        {
            auto p = owner -> physical -> getRoom() -> getPlayer( event.noun );

            if (!p)
            {
                owner -> networked -> addResponse( ColorString("Player not found!\n", Color::Red) );
                return;
            }
            
            std::stringstream whisper;

            whisper << owner -> name << " whispered to you: " << std::quoted(owner -> notifier -> event.extra) << '\n';
            
            p -> networked -> addResponse( ColorString( whisper.str(), Color::Yellow ) );
        }

        else if ( event.verb == "exit" )
        {
            addResponse("Goodbye!\n");

            setState(State::LoggedOut);
        }

        else if ( event.verb.size() && state == State::Entering )
        {
            addResponse( ColorString("You need to log in!\n", Color::Red) );
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
