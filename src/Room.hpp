#ifndef ROOM_HPP
#define ROOM_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <list>


#include "Thing.hpp"

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT

};


class Room : public Thing
{
public:
    Room(int x, int y): x(x), y(y) 
    {

    }

    static std::shared_ptr<Room> get(std::int32_t x, std::int32_t y);

    int x, y;

    void removePlayer(std::shared_ptr<Thing> player);
        
    void addPlayer(std::shared_ptr<Thing> player);
    
public:

    static std::unordered_map< std::int64_t, std::shared_ptr< Room > > mapRooms;

    std::shared_ptr<Room> roomUp    = nullptr;
    std::shared_ptr<Room> roomDown  = nullptr;
    std::shared_ptr<Room> roomLeft  = nullptr;
    std::shared_ptr<Room> roomRight = nullptr;
    
    std::list< std::shared_ptr<Thing> > listPlayers;
    std::list< std::shared_ptr<Thing> > listThings;
};



#endif//ROOM_HPP
