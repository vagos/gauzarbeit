#ifndef WORLD_HPP
#define WORLD_HPP

#include <list>
#include <memory>

class Server;
class Room;

class Thing;
class Player;

class World
{

    //std::list< std::shared_ptr<Thing> > listThings;
    std::list< std::shared_ptr<Thing> > listPlayers; // A list of all the online players.

    void addPlayer(std::shared_ptr<Thing> player)
    {
        listPlayers.push_back(player);
    }

    std::shared_ptr<Thing> getPlayer(const std::string& name);
    
    friend Server;

};


#endif//WORLD_HPP
