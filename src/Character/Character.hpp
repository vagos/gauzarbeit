#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "../Thing.hpp"
#include "../Script/ScriptedThing.hpp"

#include "../Quest.hpp"
#include <memory>

class ShopNotifier : public Notifier
{
   void onNotify(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &actor, Event::Type notification_type) override
   {
       if ( notification_type == Event::Type::Say ) 
       {
            auto event = actor -> notifier -> event;

            if (event.noun == "give")
            {
                actor -> physical -> gainItem( owner -> physical -> inventory[0] ); 
                actor -> networked -> addResponse("Here is your item!\n");
            }
       }
   }

};


class TestTalker : public Talker
{

    void doUpdate(const std::shared_ptr<Thing> &owner) override
    {

    }

};

class Character : public Thing
{

public:
    Character(const std::string& name): Thing(name)
    {
        physical = std::make_shared<Physical>();
        notifier = std::make_shared<ShopNotifier>();
        talker = std::make_shared<TestTalker>();

        physical -> gainItem( std::make_shared<ScriptedThing>("Winston") );
    }


};

#endif
