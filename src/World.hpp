#ifndef WORLD_HPP
#define WORLD_HPP

#include <bits/c++config.h>
#include <list>
#include <memory>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <regex>

#include "Player/PlayerNetworked.hpp"
#include "Thing.hpp"

class Server;
class Room;

class Player;

class World
{

public:

    std::map< std::size_t , std::shared_ptr<Thing> > playersOnline; // A list of all the online players.

    World()
    {
        Player::setPlayerCommands();
    }
    
    const std::shared_ptr<Thing> getPlayer(const std::string& name) const
    {
       std::regex r { name, std::regex_constants::icase };

       for (auto& [id, player] : playersOnline)
       {
           if ( std::regex_match(player->name, r) ) return player;
       }
       
       return nullptr;
    }

    void doUpdate()
    {
        updateRooms();
        
        for (auto& [name, player] : playersOnline) // Get request
            player -> networked() -> getRequest(player, *this); 
        
        for (auto& [name, player] : playersOnline) 
            player -> notifier() -> setEvent(player); 

        for (auto& [name, player] : playersOnline) // Handle request
           player -> networked() -> handleRequest(player, *this);

        for (auto& [name, player] : playersOnline) 
            player -> talker() -> doUpdate(player);

        for (auto& [name, player] : playersOnline)
            player -> physical() -> doUpdate(player, *this);
        
        for (auto& [name, player] : playersOnline)
        {
            try { player -> attackable() -> doUpdate(player); } 
            catch (std::exception& e) { HandleException(player, e); }
        }

        for (auto& [name, player] : playersOnline) // make this last
            player -> notifier() -> clearEvent();    
    
        for (auto& [name, player] : playersOnline) // Send response
            player -> networked() -> sendResponse(player);    

        //removeOfflinePlayers();

    }

    void addPlayer(std::shared_ptr<Thing> player) 
    {
        playersOnline[ player -> networked() -> getID() ] = player; 
    }

    void removePlayer(const std::shared_ptr<Thing>& player)
    {
        playersOnline.erase(player -> networked() -> getID());
    }

    private:

    void updateRooms()
    {
        for (auto& [room_id, room] : Room::mapRooms)
        {
            room -> doUpdate( *this );
        }
        
    }

    void removeOfflinePlayers()
    {
        for (auto it = playersOnline.begin(); it != playersOnline.end();)
        {
            if (!it -> second -> networked() -> isOnline())
            {
                it -> second -> networked() -> doDisconnect( it -> second );
                it = playersOnline.erase(it);
            }
            else
            {
                ++it;
            }
            
        }
    }
};
#endif//WORLD_HPP
