#ifndef THING_HPP
#define THING_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>

#include "Networked.hpp"

class World;
class Thing;
class Room;


class Physical
{

public:

    Physical()
    {
    }

    virtual void doUpdate( std::shared_ptr<Thing> owner, World& world ) {};

    virtual void doMove(std::shared_ptr<Thing> owner, int x, int y); // Move to room on coords x and y.

    std::shared_ptr<Room> getRoom()
    {
        return currentRoom;
    }
    
    void gainItem(std::shared_ptr<Thing> item)
    {
        tInventory.push_back(item);
    }

protected:
    std::shared_ptr<Room> currentRoom = nullptr;
    std::vector< std::shared_ptr<Thing> > tInventory;

};


class Usable 
{
public:
    
    virtual void onUse(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> user)
    {

    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }
};

class Attackable
{
public:

    Attackable()
    {

    }

    virtual void doAttack(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> target)
    {
    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

public:

    int max_health, current_health, attack, defense;
};



class Thing 
{
public:
    Thing(): sName{"INVALID_NAME"}
    {

    }

    Thing(const std::string& name): sName(name)
    {
    }

public:
    std::string sName; 

    virtual const std::string getInfo() const;

    std::shared_ptr<Networked> networked = nullptr;
    std::shared_ptr<Physical> physical   = nullptr;
    std::shared_ptr<Usable> usable       = nullptr;
    std::shared_ptr<Attackable> attackable = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.sName << thing.getInfo();

        return os;
    }

};

#endif//THING_HPP
