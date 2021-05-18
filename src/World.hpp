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
    
    const std::shared_ptr<Thing>& getPlayer(const std::string& name) const
    {
       std::regex r { name, std::regex_constants::icase };

       for (auto& [id, player] : playersOnline)
       {
           if ( std::regex_match(player->sName, r) ) return player;
       }
       
       return nullptr;
    }

    void doUpdate()
    {
        for (auto& [name, player] : playersOnline) // Get request
            player -> networked -> getRequest(player, *this); 
        
        for (auto& [name, player] : playersOnline) 
            player -> notifier -> setEvent(player); 

        for (auto& [name, player] : playersOnline) // Handle request
        {
           player -> networked -> handleRequest(player, *this);
      
           if (!std::static_pointer_cast<PlayerNetworked>(player -> networked) -> isOnline() )
               goto SKIP;
        }         

        for (auto& [name, player] : playersOnline) 
            player -> talker -> doUpdate(player);

        for (auto& [name, player] : playersOnline)
            player -> physical -> doUpdate(player, *this);
        
        for (auto& [name, player] : playersOnline)
            player -> attackable -> doUpdate(player);

        for (auto& [name, player] : playersOnline) // Send messages
            player -> networked -> sendMessages(player, *this);    
        

        for (auto& [name, player] : playersOnline) // Send messages
            player -> notifier -> clearEvent();    
    
        SKIP:

        for (auto& [name, player] : playersOnline) // Send response
            player -> networked -> sendResponse(player);    

        removeOfflinePlayers();
    }

    void addPlayer(const std::shared_ptr<Thing> player) 
    {
        playersOnline[player -> networked -> getID() ] = player; 
    }

    void removePlayer(const std::shared_ptr<Thing>& player)
    {
        playersOnline.erase(player -> networked -> getID());
    }

    void removeOfflinePlayers()
    {
        for (auto it = playersOnline.begin(); it != playersOnline.end();)
        {
            
            if (std::static_pointer_cast<PlayerNetworked>(it -> second -> networked) -> isOffline())
            {
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
