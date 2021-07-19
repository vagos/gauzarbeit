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

    virtual const std::string getInfo() const;

    std::shared_ptr<Networked>   _networked    = nullptr;
    std::shared_ptr<Physical>    _physical     = nullptr;
    std::shared_ptr<Usable>      _usable       = nullptr;
    std::shared_ptr<Attackable>  _attackable   = nullptr;
    std::shared_ptr<Talker>      _talker       = nullptr;
    std::shared_ptr<Notifier>    _notifier     = nullptr;
    std::shared_ptr<Achiever>    _achiever     = nullptr;
    std::shared_ptr<Tasker>      _tasker       = nullptr;
    std::shared_ptr<Inspectable> _inspectable  = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const Thing& thing)
    {
        os << thing.name << " " << thing.getInfo();

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


};

struct Event
{
    enum class Type 
    {
        Invalid,
        Say,
        Move,
        Use,
        Do,
        Look,
        Whisper,
        Chat,
        Message,
        Buy,
        Info,
        Ask,
        Inspect,
        Greet,
        Attack,
        Kill,
        Death,
        Enter,
        Leave,
        Gain,
    };

    std::string verb;
    std::string target;
    std::string object;
    
    std::string extra;

    Type type;

    std::string payload;
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
        event.verb = "";
        event.object = "";
        event.target = "";
    }

    virtual void doUpdate(const std::shared_ptr<Thing> &owner)
    {

    }

    Event event;
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

    virtual void doReward(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> completer)
    {

    }

    int getDifficulty() {return tasks.size();}


private:
   std::vector<bool> tasks;
};

class Achiever 
{
public:
    std::vector< std::shared_ptr<Thing> > quests;
    std::vector< std::shared_ptr<Thing> > completed_quests;
    
    virtual void gainQuest(std::shared_ptr<Thing> quest)
    {
        quests.push_back(quest);
    }

    const std::shared_ptr<Thing> getQuest(const std::string& q_name)
    {
       return FindByName(quests, q_name);
    }

    void doUpdate(const std::shared_ptr<Thing> &owner)
    {
        for (auto& q : quests)
        {
            if (q -> tasker() -> isCompleted())
            {
                q -> tasker() -> doReward( q, owner );

                onQuestComplete(owner, q);
            }
        }

        quests.erase( std::remove_if( quests.begin(), quests.end(), 
                    [](auto& t){return t -> tasker() -> isCompleted();}), 
                quests.end()); // Remove completed quests
    }

    virtual void onQuestComplete(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &quest)
    {
        getRewards(owner, quest -> tasker() -> getDifficulty());
        
        completed_quests.push_back(quest);
    }

    void gainXP(int extra_xp)
    {
       xp += extra_xp; 
    }

    virtual void getRewards(const std::shared_ptr<Thing> owner, int size)
    {
        gainXP(size);
    }

    int getLevel()
    {
        return xp/10;
    } 

    int getXP() {return xp;}

private:
    int xp = 0;

};



class Inspectable
{
public:
    Inspectable()
    {

    }

    virtual const std::string onInspect(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &inspector)
    {
        std::stringstream inspect;

        inspect << "Name: " << owner -> name << "\n\n\n";

        inspect << CenteredString("---") << "\n\n";

        if (!owner -> physical() -> inventory.empty())
        {
            inspect << "Inventory: \n";
            inspect << VerticalListString(owner -> physical() -> inventory, '-');
        }

        if (!owner -> achiever() -> quests.empty())
        {
            inspect << "Quests: \n";
            inspect << VerticalListString(owner -> achiever() -> quests, '-', 
                    [](const Thing& t) {return t.name;});
        }

        return HeaderString( inspect.str(), owner -> name );
    }

    virtual const std::string onHelp(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector)
    {
        return "";
    }
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
