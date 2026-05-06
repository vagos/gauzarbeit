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
#include <vector>

class Server;

class World
{

  public:
    static std::vector<std::string> spawn_table;

    std::map<std::size_t, std::shared_ptr<Thing>>
        playersOnline; // A list of all the online players.
    std::vector<std::unique_ptr<System>> systems;

    World();
    ~World();

    template <typename T> T* getSystem() const
    {
        for (const auto& system : systems)
        {
            if (auto* typed_system = dynamic_cast<T*>(system.get()))
                return typed_system;
        }

        return nullptr;
    }

    static World* getCurrent() { return current_world; }

    const std::shared_ptr<Thing> getPlayer(const std::string& name) const;
    void addPlayer(std::shared_ptr<Thing> player);
    void removePlayer(const std::shared_ptr<Thing>& player);
    void removeOfflinePlayers();

    void doUpdate();

  private:
    static World* current_world;
};
#endif // WORLD_HPP
