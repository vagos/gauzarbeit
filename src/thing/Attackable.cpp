#include "thing/Attackable.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"
#include <cassert>

void Attackable::doAttack(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& target)
{
    assert(owner->_physical && target->_physical);
    assert(owner->physical()->current_room == target->physical()->current_room);
    Log(owner->name << " is attacking " << target->name);

    owner->notifier()->doNotify(owner, Event::Type::Attack, target);

    target->attackable()->getDamaged(target, owner, dmg);
    target->attackable()->onAttack(target, owner);
}

void Attackable::onAttack(const std::shared_ptr<Thing>& owner,
                          const std::shared_ptr<Thing>& attacker)
{
    owner->notifier()->doNotify(owner, Event::Type::Attacked, attacker);

    if (current_health <= 0)
    {
        // Notify the killer about the kill.
        attacker->notifier()->onNotify(attacker, attacker, Event::Type::Kill, owner);
        onDeath(owner);
    }
}

void Attackable::onDeath(const std::shared_ptr<Thing>& owner)
{
    assert(owner->physical() && owner->physical()->current_room);

    alive = false;

    owner->notifier()->doNotify(owner, Event::Type::Death); // notify everyone about the death

    // Drop items on death
    for (auto& item : owner->physical()->inventory)
    {
        owner->physical()->dropItem(item);
    }

    owner->physical()->current_room->removeThing(owner);
    owner->physical()->current_room = nullptr;
}

void Attackable::getDamaged(const std::shared_ptr<Thing>& owner,
                            const std::shared_ptr<Thing>& attacker, int dmg)
{
    current_health -= dmg;
}
