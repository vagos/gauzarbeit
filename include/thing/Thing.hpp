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
#include "Talker.hpp"
#include "Achiever.hpp"
#include "Inspectable.hpp"

#include "Helpers.hpp"
#include "Exceptions.hpp"

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
class Thinker;

class Thing 
{
public:
    Thing(): name( "None" )
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

    std::shared_ptr<Networked>   _networked    = nullptr;
    std::shared_ptr<Physical>    _physical     = nullptr;
    std::shared_ptr<Usable>      _usable       = nullptr;
    std::shared_ptr<Attackable>  _attackable   = nullptr;
    std::shared_ptr<Talker>      _talker       = nullptr;
    std::shared_ptr<Notifier>    _notifier     = nullptr;
    std::shared_ptr<Achiever>    _achiever     = nullptr;
    std::shared_ptr<Tasker>      _tasker       = nullptr;
    std::shared_ptr<Inspectable> _inspectable  = nullptr;
    std::shared_ptr<Thinker>     _thinker      = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.name; 

        return os;
    }

    std::shared_ptr<Networked>  & networked()   {if (_networked)   return _networked;   throw MissingComponent(); }  
    std::shared_ptr<Physical>   & physical()    {if (_physical)    return _physical;    throw MissingComponent(); }
    std::shared_ptr<Usable>     & usable()      {if (_usable)      return _usable;      throw MissingComponent(); }
    std::shared_ptr<Attackable> & attackable()  {if (_attackable)  return _attackable;  throw MissingComponent(); }
    std::shared_ptr<Talker>     & talker()      {if (_talker)      return _talker;      throw MissingComponent(); }
    std::shared_ptr<Notifier>   & notifier()    {if (_notifier)    return _notifier;    throw MissingComponent(); }
    std::shared_ptr<Achiever>   & achiever()    {if (_achiever)    return _achiever;    throw MissingComponent(); }
    std::shared_ptr<Tasker>     & tasker()      {if (_tasker)      return _tasker;      throw MissingComponent(); }
    std::shared_ptr<Inspectable>& inspectable() {if (_inspectable) return _inspectable; throw MissingComponent(); }
    std::shared_ptr<Thinker>    & thinker()     {if (_thinker)     return _thinker;     throw MissingComponent(); }


};




class Notifier
{
public:
    
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
        event.verb   = "";
        event.object = "";
        event.target = "";
        event.extra = "";
    }

    virtual void doUpdate(const std::shared_ptr<Thing> &owner)
    {

    }

    Event event;
};

class Tasker
{


public:

    struct Task 
    {
        Task(std::string desc): description(desc), tick(false)
        {
        }

        bool tick;
        std::string description;
    };

    int addTask(std::string description)
    {
        tasks.push_back(Task( description ));

        return tasks.size() - 1;
    }

    void tickTask(int i_task)
    {
        tasks[i_task].tick =  true;
    }

    bool isCompleted()
    {
        return std::find_if(tasks.begin(), tasks.end(), 
                [](const Task& t) {return !t.tick;}) == tasks.end();
    }

    virtual void doReward(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer)
    {

    }

    int getDifficulty() {return tasks.size();}

    std::vector<Task> tasks;
};


class Thinker 
{
public:
    Thinker()
    {
    }

    virtual void doThink(const std::shared_ptr<Thing> &owner)
    {

    }

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
