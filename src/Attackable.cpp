#include "Attackable.hpp"
#include "Thing.hpp"
#include "Room.hpp"

#include <cassert>


void Attackable::doAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &target)
{
   target -> attackable -> current_health -= attack;

   target -> attackable -> onAttack(target, owner);
}

void Attackable::onDeath(const std::shared_ptr<Thing> &owner)
{
    assert(owner -> physical && owner -> physical -> current_room);
    owner -> physical -> current_room -> removeThing( owner );
}

void Attackable::onAttack(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &attacker)
{
    if (current_health <= 0) 
    {
        assert(attacker -> notifier && owner -> notifier);

        attacker -> notifier -> onNotify( attacker, owner, Notifier::Event::Type::Kill );
        owner -> notifier -> doNotify(owner, Notifier::Event::Type::Death); // notify everyone about the death
        
        onDeath(owner);
    }
}
