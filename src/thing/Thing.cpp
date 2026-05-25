#include "thing/Thing.hpp"
#include "Room.hpp"
#include "World.hpp"

std::vector<std::shared_ptr<Thing>> Thing::things; // TODO: Use this
std::size_t Thing::lastID = 0;

Thing::Thing() : Thing("A Thing") {}

Thing::Thing(const std::string& name) : name(name), id(lastID)
{
    lastID++;
}

void Thing::doUpdate(World& world)
{
    if (world.getCurrentTime() < next_update_time)
        return;

    next_update_time = world.getCurrentTime() + 1.0;

    if (_physical)
        _physical->doUpdate(shared_from_this(), world);
    if (_usable)
        _usable->doUpdate(shared_from_this());
    if (_attackable)
        _attackable->doUpdate(shared_from_this());
    if (_thinker)
        _thinker->doThink(shared_from_this(), world);
    if (_notifier)
        _notifier->doUpdate(shared_from_this());
    if (_tasker)
        _tasker->doUpdate(shared_from_this());

    if (_notifier)
        _notifier->clearEvent();
}

void Thinker::doThink(const std::shared_ptr<Thing>& owner, World& world)
{
    (void)world;

    if (!owner || !owner->_notifier || owner->notifier()->event.type != Event::Type::Attacked)
        return;

    auto event_target = owner->notifier()->event.target;
    if (event_target.empty())
    {
        owner->notifier()->clearEvent();
        return;
    }

    if (!owner->_physical || !owner->physical()->current_room)
    {
        owner->notifier()->clearEvent();
        return;
    }

    auto attacker = owner->physical()->current_room->getAnything(event_target);
    if (!attacker || !attacker->_attackable || !owner->_attackable)
    {
        owner->notifier()->clearEvent();
        return;
    }

    owner->attackable()->getDamaged(owner, attacker, attacker->attackable()->getDamage(attacker));
    owner->attackable()->onAttack(owner, attacker);

    if (owner->attackable()->is_alive() && attacker != owner && attacker->_attackable &&
        attacker->_physical && attacker->physical()->current_room == owner->physical()->current_room)
    {
        owner->attackable()->doAttack(owner, attacker);
    }

    owner->notifier()->clearEvent();
}
