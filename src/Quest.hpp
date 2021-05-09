#ifndef QUEST_HPP
#define QUEST_HPP

#include <memory>
#include <string>

#include "Thing.hpp"

class Quest
{
public:
    const std::string name;
    std::string description;

    virtual void getNotified(std::shared_ptr<Thing> owner, Notifier::Event::Type notification_type, std::shared_ptr<Thing> target)
    {
        
    }


    void doComplete(std::shared_ptr<Thing> actor)
    {

    }

};


#endif//QUEST_HPP
