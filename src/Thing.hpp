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

#include "Networked.hpp"

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
    class Event
    {
    public:

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
        };
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

    //void setEvent(std::shared_ptr<Thing> owner, Event::Type type, std::string eventInfo) 
    //{
    ////    last = Event( owner, type, eventInfo );
    //}

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

    //Event last;
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
