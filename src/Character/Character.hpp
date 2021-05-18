#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "../Thing.hpp"
#include "../Script/ScriptedThing.hpp"

class ShopNotifier : public Notifier
{
   void onNotify(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> actor, Event::Type notification_type) override
   {
       if ( notification_type == Event::Type::Say ) 
       {
            std::stringstream phrase { actor -> networked -> getRequestStream().str() } ;

            phrase.ignore( 20, ' ' ); // Ignore the first word.

            actor -> physical -> gainItem( owner -> physical -> tInventory[0] ); 

            actor -> networked -> addResponse("Here is your item!\n");
       }
   }

};

class Character : public Thing
{

public:
    Character(const std::string& name): Thing(name)
    {
        physical = std::make_shared<Physical>();
        notifier = std::make_shared<ShopNotifier>();

        physical -> gainItem( std::make_shared<ScriptedThing>("Winston") );
    }


};

#endif
