#ifndef NETWORKED_HPP
#define NETWORKED_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <bits/c++config.h>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>

class Thing;
class World;

class Networked 
{
public:

    Networked() 
    {
        socket = std::make_unique<sf::TcpSocket>();
        ID = lastID; ++lastID;
    }

    std::unique_ptr<sf::TcpSocket> socket;

    virtual void doUpdate(std::shared_ptr<Thing> owner) {}
    virtual void handleRequest(std::shared_ptr<Thing> owner, World& world) {}
    virtual void getRequest(std::shared_ptr<Thing> owner, World& world) {}
    virtual void sendResponse(std::shared_ptr<Thing> owner) {}
    virtual void doDatabaseLoad(std::shared_ptr<Thing> owner) {}

    
    void addResponse(const std::string& res)
    {
       streamResponse << res ; 
    }

    void doDisconnect(const std::shared_ptr<Thing> &owner);

    const std::stringstream& getRequestStream() { return streamRequest; }

    std::size_t getID() {return ID;}

    virtual bool isOnline() {return online;}
    virtual bool isOffline() {return false;}

protected:
    
    void clearStreams()
    {
        streamRequest.str(std::string()); 
        streamResponse.str(std::string()); 
    }

    char cData[100];
    std::size_t nReceived;

    std::size_t ID;
    static std::size_t lastID; 

    std::stringstream streamRequest;
    std::stringstream streamResponse;

    bool online = false;

};

#endif//NETWORKED_HPP
