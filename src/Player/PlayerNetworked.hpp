#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

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

    PlayerNetworked()
    {

    }

    void getRequest( std::shared_ptr<Thing> owner, World& world )
    {
            streamRequest.str(Server::getMessage( *socket ));

            if (!streamRequest.str().size()) return;

            std::clog << *owner << ": " << streamRequest.str() << " Received: " << streamRequest.str().size() << " bytes" << "\n";
    }
    
    void sendResponse(std::shared_ptr<Thing> owner) override
    {
        if ( ! streamResponse.str().size() ) goto CLEAR;

        Server::sendMessage(*socket, streamResponse.str());
        
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
