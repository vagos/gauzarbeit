#ifndef PLAYER_NETWORKED_HPP
#define PLAYER_NETWORKED_HPP

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>
#include <experimental/filesystem>

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

            std::clog << *owner << ": " << streamRequest.str() << '\r' << " Received: " << streamRequest.str().size() << " bytes" << "\n";
    }
    
    void sendResponse(std::shared_ptr<Thing> owner) override
    {
        if ( ! (streamResponse.str().size() || streamRequest.str().size()) ) goto CLEAR;

        addResponse(">> ");

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

                doDatabaseStore(owner);

                doDisconnect(owner);

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
        std::string filename{"./db/players/" + owner -> name};

        if (!std::experimental::filesystem::exists(filename)) return;

        db.open(filename);

        if (!db.is_open()) 
        {
            std::clog << "Player not found!\n";
            return;
        }

        std::string line;

        db >> line >> line >> line;

        // Load INVENTORY
        {
            db >> line;

            while(line != "END" && !db.eof())
            {

                auto t = std::make_shared<ScriptedThing>(line);

                t -> networked() -> doDatabaseLoad( t );

                owner -> physical() -> gainItem( t );

                db >> line;
            }
        }

        // Load STATS

        {
            db >> line;

            int stat;

            db >> stat;
            
            owner -> achiever() -> gainXP(stat);

            db >> owner -> attackable() -> current_health;
            db >> owner -> attackable() -> attack;
            db >> owner -> attackable() -> defense;

            db >> line;

            std::clog << line << '\n';
        }


        db.close();
    }

    const std::string doDatabaseSave(std::shared_ptr<Thing> owner) override
    {
        std::stringstream info;

        info << "PLAYER: " << owner -> name << '\n';

        info << "INVENTORY\n";

        for (auto& t : owner -> physical() -> inventory)
        {
            info << t -> name << ' ' << t -> networked() -> doDatabaseSave(t) << '\n';
        }

        info << "END\n";

        info << "STATS\n";

        info << owner -> achiever() -> getLevel() << ' ';

        info << owner -> attackable() -> current_health << ' ';
        info << owner -> attackable() -> attack << ' ';
        info << owner -> attackable() -> defense << ' ';

        info << '\n';

        info << "END\n";

        return info.str();
    }

    void doDatabaseStore(std::shared_ptr<Thing> owner) 
    {
        std::string filename{"./db/players/" + owner -> name};
    
        db.open(filename, std::ios::trunc);

        if (!db.is_open()) 
        {
            std::ofstream file{filename};
            db.open(filename);
        }

        db << doDatabaseSave(owner);
        
        db.close();

        std::clog << "Player " << owner -> name << " saved!\n";
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
