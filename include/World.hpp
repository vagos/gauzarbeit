#ifndef WORLD_HPP
#define WORLD_HPP

#include <cstddef>
#include <list>
#include <memory>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <regex>

#include "thing/Thing.hpp"
#include "player/Player.hpp"
#include "Room.hpp"

class Server;


class World
{

public:

    std::map< std::size_t , std::shared_ptr<Thing> > playersOnline; // A list of all the online players.

    World();
    
    const std::shared_ptr<Thing> getPlayer(const std::string& name) const;
    void addPlayer(std::shared_ptr<Thing> player);
    void removePlayer(const std::shared_ptr<Thing>& player);
    void removeOfflinePlayers();
    
    void doUpdate();
    void updateRooms();
    
};
#endif//WORLD_HPP
