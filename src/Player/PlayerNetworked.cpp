#include "PlayerNetworked.hpp"

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
