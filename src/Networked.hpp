#ifndef NETWORKED_HPP
#define NETWORKED_HPP

#include <bits/c++config.h>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Thing;
class World;

class Networked 
{
public:

    Networked()
    {
        ID = lastID++;
    }

    std::unique_ptr<tcp::socket> socket;

    virtual void doUpdate(std::shared_ptr<Thing> owner) {}

    virtual void handleRequest(std::shared_ptr<Thing> owner, World& world) {}
    virtual void getRequest(std::shared_ptr<Thing> owner, World& world) {}
    virtual void sendResponse(std::shared_ptr<Thing> owner) {}

    virtual void doDatabaseLoad(std::shared_ptr<Thing> owner) {} // Maybe add parent
    virtual const std::string doDatabaseSave(std::shared_ptr<Thing> owner) {return "";}
    
    
    void addResponse(const std::string& res)
    {
       streamResponse << res ; 
    }

    void doDisconnect(const std::shared_ptr<Thing> &owner);

    std::stringstream& getRequestStream() { return streamRequest; }

    std::size_t getID() {return ID;}

    bool isOnline() {return online;}

    void setOnline(bool o) {online = o;}

    static std::fstream& getDB() {return db;}

protected:
    
    void clearStreams()
    {
        streamRequest.str(std::string()); 
        streamResponse.str(std::string()); 
    }

    std::size_t ID;
    static std::size_t lastID; 

    std::stringstream streamRequest;
    std::stringstream streamResponse;

    static std::fstream db;

    bool online = false;

};

#endif//NETWORKED_HPP
