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

class PlayerNetworked : public Networked 
{

public:

    PlayerNetworked()
    {

    }

    void getRequest( std::shared_ptr<Thing> owner, World& world )
    {
        try 
        {
            streamRequest.str(Server::getMessage( *socket ));
        }

        catch (PlayerDisconnect& error)
        {
            doDatabaseStore(owner);
            doDisconnect(owner);
        }

        if (!streamRequest.str().size()) return;

        std::clog << *owner << ": " << streamRequest.str() << "\r\r\r" << " Received: " << streamRequest.str().size() << " bytes" << "\n";
    }
    
    void sendResponse(std::shared_ptr<Thing> owner) override
    {
        if ( ! (streamResponse.str().size() || streamRequest.str().size()) ) goto CLEAR;

        addResponse(">> ");

        Server::sendMessage(*socket, streamResponse.str());
        
        CLEAR:
            
        clearStreams();
    }
    
    void handleRequest(std::shared_ptr<Thing> owner, World& world) override; 

    void doDatabaseLoad(std::shared_ptr<Thing> owner) override
    {
        if (!inDatabase(owner -> name)) return;

        std::string filename{"./db/players/" + owner -> name};
        db.open(filename);

        if (!db.is_open()) 
        {
            std::clog << "Player not found!\n";
            return;
        }

        std::string line;

        db >> line >> line >> line; 

        db >> password;

        db >> line;

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
        }


        db.close();
    }

    const std::string doDatabaseSave(std::shared_ptr<Thing> owner) override
    {
        std::stringstream info;

        info << "PLAYER: " << owner -> name << '\n';

        info << "PASSWORD " << password << '\n';

        info << "INVENTORY\n";

        for (auto& t : owner -> physical() -> inventory)
        {
            info << t -> name << ' ' << t -> networked() -> doDatabaseSave(t) << '\n';
        }

        info << "END\n";

        info << "STATS\n";

        info << owner -> achiever() -> getXP() << ' ';

        info << owner -> attackable() -> current_health << ' ';
        info << owner -> attackable() -> attack << ' ';
        info << owner -> attackable() -> defense << ' ';

        info << '\n';

        info << "END\n";

        return info.str();
    }

    void doDatabaseStore(std::shared_ptr<Thing> owner) override 
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

    bool inDatabase(const std::string &name)
    {
        std::string filename{"./db/players/" + name};
        return std::experimental::filesystem::exists(filename);
    }

private: 
    std::string password;
};

#endif
