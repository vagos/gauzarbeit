#ifndef THING_HPP
#define THING_HPP

#include <bits/c++config.h>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>
#include <cassert>

#include "Networked.hpp"
#include "Physical.hpp"
#include "Attackable.hpp"

#include "Helpers.hpp"

class World;
class Thing;
class Room;
class Physical;
class Usable;
class Attackable;
class Talker;
class Notifier;
class Achiever;
class Tasker;
class Inspectable;

class Thing 
{
public:
    Thing(): name( "INVALID_NAME" )
    {

    }

    ~Thing()
    {
        std::clog << name << " got destroyed!\n";
    }

    Thing(const std::string& name): name(name)
    {
    }

public:
    std::string name; 

    virtual const std::string getInfo() const;

    std::shared_ptr<Networked>   networked    = nullptr;
    std::shared_ptr<Physical>    physical     = nullptr;
    std::shared_ptr<Usable>      usable       = nullptr;
    std::shared_ptr<Attackable>  attackable   = nullptr;
    std::shared_ptr<Talker>      talker       = nullptr;
    std::shared_ptr<Notifier>    notifier     = nullptr;
    std::shared_ptr<Achiever>    achiever     = nullptr;
    std::shared_ptr<Tasker>      tasker       = nullptr;
    std::shared_ptr<Inspectable> inspectable  = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.name << " " << thing.getInfo();

        return os;
    }

};

class Tasker
{

public:

    int addTask()
    {
        tasks.push_back(false);

        return tasks.size() - 1;
    }

    void tickTask(int i_task)
    {
        tasks[i_task] = true;
    }

    bool isCompleted()
    {
        return std::find_if(tasks.begin(), tasks.end(), 
                [](const auto& t) {return !t;}) == tasks.end();
    }

    virtual void giveRewards(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer)
    {

    }

private:
   std::vector<bool> tasks;
};


class Inspectable
{
public:
    Inspectable()
    {

    }

    virtual const std::string onInspect(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector)
    {
        if (owner -> physical)
        {
            return VerticalListString(owner -> physical -> inventory, '*');
        }

        return description;

    }
private:
    std::string description;

};

class Achiever 
{
public:
    std::vector< std::shared_ptr<Thing> > quests;
    std::vector< std::shared_ptr<Thing> > completed_quests;
    
    void addQuest( std::shared_ptr<Thing> quest)
    {
        quests.push_back(quest);
    }

    virtual void doUpdate(std::shared_ptr<Thing> owner)
    {

    }

    void gainXP(int extra_xp)
    {
       xp += extra_xp; 
    }

    int getLevel()
    {
        return xp/10;
    }

private:
    int xp = 0;

};


class Talker // This component can give quests/dialog
{
public:

    Talker()
    {

    }

    virtual void doUpdate(const std::shared_ptr<Thing>& owner)
    {

    }

    virtual void onTalk(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing> talker)
    {

    }

protected:
    std::vector< std::string > givable_quests;

};

class Notifier
{
public:
    struct Event
    {
        enum class Type 
        {
            Invalid,
            Catch,
            Say,
            Kill,
            Death,
            Give,
            Move,
            Use,
            Look,
            Whisper,
            Chat,
            Message,
            Buy,
            Info,
            Ask,
        };

        std::string verb;
        std::string target;
        std::string object;
        
        std::string extra;

        std::string payload;
    };

    Notifier() 
    {

    }

    virtual void doNotify(const std::shared_ptr<Thing>& owner, Event::Type notification_type, const std::shared_ptr<Thing>& target = nullptr)
    {


    }

    virtual void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor, Event::Type notification_type, const std::shared_ptr<Thing>& target = nullptr)
    {

    }

    virtual void setEvent(const std::shared_ptr<Thing>& owner)
    {

    }

    void setEventPayload(std::string p)
    {
        event.payload = p;
    }

    void clearEvent()
    {
        event.verb = "";
        event.object = "";
        event.target = "";
    }

    virtual void doUpdate(const std::shared_ptr<Thing> &owner)
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
    
    virtual void onUse(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &user)
    {

    }

    virtual void doUpdate(const std::shared_ptr<Thing> &owner)
    {

    }
};

#endif//THING_HPP
