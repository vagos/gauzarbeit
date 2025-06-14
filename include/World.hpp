#ifndef WORLD_HPP
#define WORLD_HPP

#include "Room.hpp"
#include "System.hpp"
#include "player/Player.hpp"
#include "thing/Thing.hpp"

#include <algorithm>
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>

class Server;

class World
{

  public:
    std::map<std::size_t, std::shared_ptr<Thing>>
        playersOnline; // A list of all the online players.
    std::vector<std::unique_ptr<System>> systems;

    World();

    const std::shared_ptr<Thing> getPlayer(const std::string& name) const;
    void addPlayer(std::shared_ptr<Thing> player);
    void removePlayer(const std::shared_ptr<Thing>& player);
    void removeOfflinePlayers();

    void doUpdate();
    void updateRooms();
};
#endif // WORLD_HPP
