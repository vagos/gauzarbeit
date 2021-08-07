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

#include "Thing.hpp"
#include "Player/Player.hpp"
#include "Room.hpp"

class Server;


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
        
        for (auto& [name, player] : playersOnline) 
        {
            try  { player -> notifier() -> setEvent(player); } 
            catch (std::exception& e) { HandleException(player, e); }
        }
            
        for (auto& [name, player] : playersOnline) 
        {
            try { player -> talker() -> doUpdate(player); }
            catch (std::exception& e) { HandleException(player, e); }
        }
        for (auto& [name, player] : playersOnline)
        {
            try { player -> physical() -> doUpdate(player, *this); }
            catch (std::exception& e) { HandleException(player, e); }
        }
        
        for (auto& [name, player] : playersOnline)
        {
            try { player -> attackable() -> doUpdate(player); } 
            catch (std::exception& e) { HandleException(player, e); }
        }
        
        for (auto& [name, player] : playersOnline)
        {
            try { player -> achiever() -> doUpdate(player); } 
            catch (std::exception& e) { HandleException(player, e); }
        }

        for (auto& [name, player] : playersOnline) // make this last
            player -> notifier() -> clearEvent();    
    
        removeOfflinePlayers(); // TODO move this to server
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
