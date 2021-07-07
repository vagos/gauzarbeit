#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>

#include "../Thing.hpp"
#include "../Server.hpp"

#include "PlayerPhysical.hpp"
#include "PlayerAttackable.hpp"
#include "PlayerNotifier.hpp"
#include "PlayerTalker.hpp"
#include "PlayerInspectable.hpp"

class World;

class PlayerNetworked : public Networked 
{

public:

    PlayerNetworked(): state(State::Entering)
    {
        //state = State::LoggedIn;
        //online = true;
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

        if (!nReceived) return;

        if ( cData[nReceived - 1] != '\n' ) return;
        
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
        auto& event = owner -> notifier() -> event;

        switch (event.type)
        {
            case Event::Type::Enter:
            {
                if ( isOnline() )
                {
                    addResponse( ColorString("You are already logged in!\n", Color::Red) );
                    return;
                }

                owner -> name = owner -> notifier() -> event.target;

                setState( State::LoggedIn ); 

                online = true;

                addResponse( ColorString("You are logged in as " + owner -> name + ".\n", Color::Green) );

                doDatabaseLoad(owner);
                
                break;
            }

            case Event::Type::Leave:
            {
                 addResponse("Goodbye!\n");

                setState(State::LoggedOut);

                break;
            }

            default:
            {
                if ( !isOnline() && event.verb.size() )  
                   addResponse( ColorString("You need to log in!\n", Color::Red) );
            }


        }
    }

    void doDatabaseLoad(std::shared_ptr<Thing> owner) override
    {
    }



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
