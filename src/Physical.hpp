#ifndef PHYSICAL_HPP
#define PHYSICAL_HPP

#include <memory>
#include <algorithm>
#include <string>
#include <vector>

class World;
class Room;
class Thing;

class Physical
{

public:

    Physical()
    {
    }

    virtual void doUpdate( const std::shared_ptr<Thing> &owner, World& world ) {};

    virtual void doMove(std::shared_ptr<Thing> owner, int x, int y); // Move to room on coords x and y.

    std::shared_ptr<Room> getRoom()
    {
        return current_room;
    }
    
    void gainItem(std::shared_ptr<Thing> item)
    {
        inventory.push_back(item);
    }

    void giveItem(std::shared_ptr<Thing> target, std::shared_ptr<Thing> item);

    std::shared_ptr<Thing> getItem(std::string item_name);
        
    std::shared_ptr<Thing> getItem(int item_index)
    {
        return inventory[item_index];
    }

    void loseItem(std::shared_ptr<Thing> item)
    {
        inventory.erase( std::remove(inventory.begin(), inventory.end(), item), inventory.end() );
    }

    void dropItem(std::shared_ptr<Thing> item);

    bool hasItem(std::shared_ptr<Thing> item)
    {
        return std::find( inventory.begin(), inventory.end(), item ) != inventory.end();
    }

    void equipItem( std::shared_ptr<Thing> item )
    {
        equipment.push_back(item);
        loseItem(item);
    }

    std::shared_ptr<Room> current_room = nullptr;
    std::vector< std::shared_ptr<Thing> > inventory;
    std::vector< std::shared_ptr<Thing> > equipment;
    
};

#endif//PHYSICAL_HPP
