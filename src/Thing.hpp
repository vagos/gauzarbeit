#ifndef THING_HPP
#define THING_HPP
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>


class World;
class Thing;
class Room;


class Networked 
{
public:

    Networked() 
    {
        socket = std::make_unique<sf::TcpSocket>();
    }

    std::unique_ptr<sf::TcpSocket> socket;

    virtual void doUpdate(std::shared_ptr<Thing> owner) {}
    virtual void handleRequest(std::shared_ptr<Thing> owner) {}

    void clearStreams()
    {
        streamRequest.str(std::string()); 
        streamResponse.str(std::string()); 
    }
    
    void addResponse(const std::string& res)
    {
       streamResponse << res; 
    }

    const std::stringstream& getRequestStream() { return streamRequest; }


    char cData[100];
    std::size_t nReceived;

    std::stringstream streamRequest;
    std::stringstream streamResponse;


};

class Physical
{

public:

    Physical()
    {
    }

    virtual void doUpdate( std::shared_ptr<Thing> owner ) {};

    void doMove(std::shared_ptr<Thing> owner, int x, int y); // Move to room on coords x and y.

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
    
    virtual void doUse(std::shared_ptr<Thing> user)
    {

    }

};



class Thing 
{
public:
    Thing(): sName{"INVALID_NAME"}
    {

    }

public:
    std::string sName; 

    void doUpdate(World& world);

    virtual const std::string getInfo() const;

    std::shared_ptr<Networked> networked = nullptr;
    std::shared_ptr<Physical> physical   = nullptr;
    std::shared_ptr<Usable> usable   = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.sName << thing.getInfo();

        return os;
    }

};

#endif//THING_HPP
