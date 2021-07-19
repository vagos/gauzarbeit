#ifndef ROOM_HPP
#define ROOM_HPP

#include <cstdint>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <list>
#include <random>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <memory>

#include "Thing.hpp"

class Player;
class RoomInspectable;

class Room : public Thing
{
public:
    Room(int x, int y): x(x), y(y) 
    {
        //inspectable = std::make_unique<RoomInspectable>();
    }

    static std::shared_ptr<Room> get(std::int32_t x, std::int32_t y);

    int x, y;

    void removePlayer(std::shared_ptr<Thing> player);
    void removeThing(std::shared_ptr<Thing> thing);
        
    void addPlayer(std::shared_ptr<Thing> player);
    void addThing(std::shared_ptr<Thing> thing);

    const std::shared_ptr<Thing> getThing(const std::string& name);
    const std::shared_ptr<Thing> getPlayer(const std::string& name);

    const std::shared_ptr<Thing> getAnything(const std::string& name);

    const std::string onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector);
    


    virtual void doUpdate(World& world);

    virtual void doGeneration()
    {

    }

public:

    static std::unordered_map< std::int64_t, std::shared_ptr< Room > > mapRooms;
    
    std::vector< std::shared_ptr<Thing> > players;
    std::vector< std::shared_ptr<Thing> > things;
};

class ScriptedRoom : public Room
{
public:
    ScriptedRoom(int x, int y) : Room(x, y)
    {
    }

    void doUpdate(World &world) override; 

    void doGeneration() override;
};

#endif//ROOM_HPP
