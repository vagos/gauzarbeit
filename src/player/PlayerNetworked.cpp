#include "player/PlayerNetworked.hpp"
#include "player/PlayerPhysical.hpp"

void PlayerNetworked::handleRequest(std::shared_ptr<Thing> owner, World &world)
{
    std::string verb, target, object;    
    std::stringstream req{ streamRequest.str() };
    
    req >> verb >> target >> object;

    if (verb == "login")
    {
        if ( isOnline() )
        {
            addResponse( ColorString("You are already logged in!\n", Color::Red) );
            return;
        }

        if (!inDatabase( target ))
        {
            addResponse( "You need to register first!\n" );
            return;
        }

        owner -> name = target;

        doDatabaseLoad(owner);

        if ( password != object )
        {
            addResponse( "Wrong password! Please try again.\n" );
            doDisconnect(owner);
            return;
        }

        setOnline(true);
        
        addResponse( ColorString("You are logged in as " + owner -> name + ".\n", Color::Green) );
    }

    else if (verb == "register")
    {
        if ( inDatabase(target) )
        {
            addResponse( ColorString("A player with that name already exists!\n", Color::Red) );
        }

        owner -> name = target;
        password = object; 

        addResponse( "You are registered!\n" );
        addResponse( ColorString("You are logged in as " + owner -> name + ".\n", Color::Green) );

        doDatabaseStore(owner);

        setOnline(true);
    }
    else
    {
        if ( !isOnline() && verb.size() )  
           addResponse( ColorString("You need to log in!\n", Color::Red) );
    }
}

PlayerNetworked::PlayerNetworked()
{

}

void PlayerNetworked::getRequest( std::shared_ptr<Thing> owner, World& world )
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

void PlayerNetworked::sendResponse(std::shared_ptr<Thing> owner) 
{
    if ( ! (streamResponse.str().size() || streamRequest.str().size()) ) goto CLEAR;

    addResponse(">> ");

    Server::sendMessage(*socket, streamResponse.str());
    
    CLEAR:
        
    clearStreams();
}

void PlayerNetworked::doDatabaseLoad(std::shared_ptr<Thing> owner)
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
        float val;

        db >> line;

        db >> val; // Load XP

        owner -> achiever() -> gainXP( val );

        db >> line;

        while (line != "END" && !db.eof())
        {
           db >> val;

           owner->achiever()->setStat(line, val);

           db >> line;
        }

        db >> line;

        std::clog << "LINE: " << line << '\n';
    }


    db.close();
}

const std::string PlayerNetworked::doDatabaseSave(std::shared_ptr<Thing> owner) 
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

    for (auto [stat_name, stat] : owner -> achiever() -> extra_stats)
    {
        info << stat_name << ' ' << stat -> value << '\n';
    }

    info << "END\n";

    info << '\n';

    info << "END\n";

    return info.str();
}

void PlayerNetworked::doDatabaseStore(std::shared_ptr<Thing> owner) 
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

bool PlayerNetworked::inDatabase(const std::string &name)
{
    std::string filename{"./db/players/" + name};
    return std::experimental::filesystem::exists(filename);
}
