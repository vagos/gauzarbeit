#ifndef THING_HPP
#define THING_HPP
#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <iostream>
#include <SFML/Network/TcpSocket.hpp>
#include <string>


class World;
class Thing;
class Room;

class Component
{
public:

    Component(std::shared_ptr<Thing> owner): owner(owner)
    {

    }

    std::shared_ptr<Thing> owner;
};

class Networked : public Component 
{
public:

    Networked(std::shared_ptr<Thing> owner): Component(owner)
    {
        socket = std::make_unique<sf::TcpSocket>();
    }

    std::unique_ptr<sf::TcpSocket> socket;

    virtual void doUpdate(sf::SocketSelector& socketSelector)
    {
    }

};

class Physical: public Component
{

public:

    Physical(std::shared_ptr<Thing> owner): Component(owner)
    {
    }

    virtual void doUpdate() {};

    void doMove(int x, int y); // Move to room on coords x and y.

    std::shared_ptr<Room>& getRoom()
    {
        return currentRoom;
    }

protected:
    std::shared_ptr<Room> currentRoom = nullptr;
    std::vector< std::shared_ptr<Thing> > vInventory;

};



class Thing : public std::enable_shared_from_this<Thing>
{
public:
    Thing(): sName{"INVALID_NAME"}
    {

    }

public:
    std::string sName; 

    void doUpdate(World& world);

    std::unique_ptr<Networked> networked = nullptr;
    std::unique_ptr<Physical> physical   = nullptr;
    
    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.sName;

        return os;
    }

};

#endif//THING_HPP
