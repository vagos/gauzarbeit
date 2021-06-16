#include "Attackable.hpp"
#include "Thing.hpp"
#include "Room.hpp"

#include <cassert>


void Attackable::doAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &target)
{
   assert( owner -> physical && target -> physical ); //testing

   if (owner -> physical -> current_room != target -> physical -> current_room) return;

   target -> attackable -> current_health -= attack;

   target -> attackable -> onAttack(target, owner);
}

void Attackable::onDeath(const std::shared_ptr<Thing> &owner)
{
    assert(owner -> physical && owner -> physical -> current_room);

    alive = false;

    owner -> notifier -> doNotify(owner, Notifier::Event::Type::Death); // notify everyone about the death

    for ( auto& item : owner -> physical -> inventory )
    {
        owner -> physical -> current_room -> addThing(item);
    }

//    owner -> physical -> current_room -> removeThing( owner );
//    owner -> physical -> current_room = nullptr;


}

void Attackable::onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker)
{
    if (current_health <= 0) 
    {
        assert(attacker -> notifier && owner -> notifier);

        attacker -> notifier -> onNotify( attacker, attacker, Notifier::Event::Type::Kill, owner );
        
        onDeath(owner);
    }
}
