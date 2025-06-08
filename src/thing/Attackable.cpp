#include "thing/Attackable.hpp"

#include "Room.hpp"
#include "thing/Thing.hpp"

#include <cassert>

void Attackable::doAttack(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& target)
{
    assert(owner->_physical && target->_physical); // testing

    if (owner->physical()->current_room != target->physical()->current_room)
        throw TargetNotFound();

    target->attackable()->getDamaged(target, owner, owner->achiever()->getStat("Attack"));

    target->attackable()->onAttack(target, owner);
}

void Attackable::onAttack(const std::shared_ptr<Thing>& owner,
                          const std::shared_ptr<Thing>& attacker)
{
    if (current_health <= 0)
    {
        // Notify the killer about the kill.
        attacker->notifier()->onNotify(attacker, attacker, Event::Type::Kill, owner);

        onDeath(owner);
    }
}

void Attackable::onDeath(const std::shared_ptr<Thing>& owner)
{
    // assert(owner -> physical() && owner -> physical() -> current_room);

    alive = false;

    // owner -> notifier -> doNotify(owner, Event::Type::Death); // notify everyone about the death

    for (auto& item : owner->physical()->inventory)
    {
        owner->physical()->current_room->addThing(item); // Drop items on death.
    }

    //    owner -> physical -> current_room -> removeThing( owner );
    //    owner -> physical -> current_room = nullptr;
}

void Attackable::getDamaged(const std::shared_ptr<Thing>& owner,
                            const std::shared_ptr<Thing>& attacker, int dmg)
{
    current_health -= dmg;
}
