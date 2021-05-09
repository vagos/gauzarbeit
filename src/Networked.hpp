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

enum class Color
{
    None,
    Red,
    Green,
    Blue,
    White,

};



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

    
    void addResponse(const std::string& res, Color color = Color::None)
    {
       streamResponse << GetColor(color) << res << GetColor(Color::White); 
    }

    void addMessage(const std::string message, const std::string recipient = "")
    {
        qMessages.push( std::pair < const std::string, const std::string >(message, recipient) );
    }

    void sendMessages(std::shared_ptr<Thing> owner, const World& world);


    const std::stringstream& getRequestStream() { return streamRequest; }

    std::size_t getID() {return ID;}

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

    std::queue< std::pair < const std::string, const std::string > > qMessages;
    
    static std::string GetColor(Color color_code)
    {
        switch (color_code)
        {
            case Color::Red:
                return "\u001b[31m"; break;
            case Color::White:
                return "\u001b[0m"; break;
            default:
                return ""; break;
        }

        return "";
    }

};

#endif//NETWORKED_HPP
