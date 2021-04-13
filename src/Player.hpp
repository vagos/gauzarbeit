#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Thing.hpp"
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
        
        switch ( state )
        {
            case State::Entering:
            {
                if (streamRequest.str().size())
                {
                    owner -> sName = streamRequest.str();
                    state = State::LoggedIn;

                    streamResponse << "Welcome, " << streamRequest.str() << "!\n";
                }

                else 
                {
                    streamResponse << "Username: ";
                }

                break;
            
            }

            case State::LoggedIn:
            {
                
            }

        }
        
        sendResponse(socketSelector);
        
    }


private: 
    
    void getRequest( sf::SocketSelector& socketSelector )
    {
        
        if ( ! socketSelector.isReady( *socket ) ) return;

        socket->receive(cData, 100, nReceived);

        if (! cData[nReceived - 1] == '\n') return;
        
        streamRequest << std::string{cData, nReceived - 2};

        std::clog << *owner << " " << streamRequest.str() << "\n";

    }

    void sendResponse( sf::SocketSelector& socketSelector )
    {
        //if ( ! socketSelector.isReady( *socket ) ) return;
        
        if (! streamResponse.str().size() ) return;

        socket -> send(streamResponse.str().data(), streamResponse.str().size());
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

class Player : public Thing
{
public:
    Player() 
    {
    }


    void doInit()
    {
       networked = std::make_unique<PlayerNetworked>(shared_from_this());
       physical = std::make_unique<Physical>(shared_from_this());
    }


};

#endif//PLAYER_HPP
