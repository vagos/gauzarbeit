#ifndef THING_HPP
#define THING_HPP

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <bits/c++config.h>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>

#include "Networked.hpp"

#include "Helpers.hpp"

class World;
class Thing;
class Room;
class Physical;
class Usable;
class Attackable;
class Talker;
class Notifier;

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
    std::shared_ptr<Talker> talker = nullptr;
    std::shared_ptr<Notifier> notifier = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.sName << thing.getInfo();

        return os;
    }

};

class Inspectable
{
public:
    Inspectable()
    {

    }


    virtual void onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector)
    {

    }

};


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

    std::shared_ptr<Thing> getItem(std::string item_name)
    {
        auto r = std::find_if(tInventory.begin(), tInventory.end(), 
                [&item_name](const std::shared_ptr<Thing> t) {return (t -> sName == item_name);}); 

        return r != tInventory.end() ? *r : nullptr;
    }
    
    std::shared_ptr<Thing> getItem(int item_index)
    {
        return tInventory[item_index];
    }

    void loseItem(std::shared_ptr<Thing> item)
    {
        tInventory.erase( std::remove(tInventory.begin(), tInventory.end(), item), tInventory.end() );
    }

    bool hasItem(std::shared_ptr<Thing> item)
    {
        return std::find( tInventory.begin(), tInventory.end(), item ) != tInventory.end();
    }

    //void giveItem(std::shared_ptr<>)

protected:
    std::shared_ptr<Room> currentRoom = nullptr;
public:
    std::vector< std::shared_ptr<Thing> > tInventory;

};

class Quest;

class Talker // This component can give quests/dialog
{
public:

    Talker()
    {

    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

    void gainQuest(std::shared_ptr<Quest> quest)
    {
        quests.push_back(quest);
    }

    void giveQuest(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> target, int questIndex = 0)
    {
        target -> talker -> gainQuest( quests[ questIndex ] );       

        quests.erase( quests.begin() + questIndex );
    }

    auto& getQuests() {return quests;}

protected:
    std::vector< std::shared_ptr<Quest> > quests;

};

class Notifier
{
public:
    struct Event
    {
        enum class Type
        {
            Invalid,
            None,
            Say,
            Kill,
            Give,
            Move,
            Use,
            Look,
            Whisper,
            Message,
        };

        std::string verb;
        std::string noun;
        std::string extra;
    };

    Notifier() 
    {

    }

    virtual void doNotify(std::shared_ptr<Thing> owner, Event::Type notification_type, std::shared_ptr<Thing> target = nullptr)
    {


    }

    virtual void onNotify(std::shared_ptr<Thing> owner, 
            std::shared_ptr<Thing> actor, Event::Type notification_type)
    {

    }

    virtual void setEvent(std::shared_ptr<Thing> owner)
    {

    }

    void clearEvent()
    {
        event.verb = "";
    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

    Event event;
};

class Thinker 
{

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

    virtual void onAttack(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> attacker) 
    {

    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

    void setMaxHealth(int max_hp)
    {
        max_health = max_hp;
        current_health = max_health;
    }

public:

    int max_health, current_health, attack = 1, defense;
};

#endif//THING_HPP
