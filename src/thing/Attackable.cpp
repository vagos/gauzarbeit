#include "thing/Attackable.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"
#include "Helpers.hpp"
#include <cassert>
#include <iterator>
#include <vector>

void Attackable::doUpdate(const std::shared_ptr<Thing>& owner)
{
    assert(owner);
    assert(owner->_physical);

    auto attack_target = target.lock();
    if (!attack_target)
        return;

    assert(attack_target != owner);
    assert(attack_target->_attackable);
    assert(attack_target->_physical);
    assert(alive);

    if (!attack_target->attackable()->is_alive() ||
        owner->physical()->current_room != attack_target->physical()->current_room)
    {
        target.reset();
        return;
    }

    doAttack(owner, attack_target);
}

void Attackable::doAttack(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& target)
{
    assert(owner->_physical && target->_physical);
    assert(owner->_notifier && target->_notifier);
    auto in_same_room = owner->physical()->current_room == target->physical()->current_room;
    if (!in_same_room) 
    {
        Log(owner->name << " cannot attack " << target->name << " because they are not in the same room.");
        return;
    }
    Log(owner->name << " is attacking " << target->name);

    Event attack_event(Event::Type::Attack, "attack", target->name);
    owner->notifier()->doNotify(owner, attack_event, target);

    target->attackable()->getDamaged(target, owner, getDamage(owner));
    target->attackable()->onAttack(target, owner);
}

void Attackable::onAttack(const std::shared_ptr<Thing>& owner,
                          const std::shared_ptr<Thing>& attacker)
{
    Event attacked_event(Event::Type::Attacked, "attacked", attacker->name);
    owner->notifier()->doNotify(owner, attacked_event, attacker);

    if (current_health <= 0)
    {
        // Notify the killer about the kill.
        Event kill_event(Event::Type::Kill, "kill", owner->name);
        attacker->notifier()->onNotify(attacker, attacker, kill_event, owner);
        onDeath(owner);
        return;
    }
    
    assert(attacker != owner);
    target = attacker;
}

void Attackable::onDeath(const std::shared_ptr<Thing>& owner)
{
    assert(owner->physical() && owner->physical()->current_room);

    alive = false;
    target.reset();
    // Notify everyone about the death
    Event death_event(Event::Type::Death, "death");
    owner->notifier()->doNotify(owner, death_event);

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

int Attackable::getDamage(const std::shared_ptr<Thing>& owner)
{
    SeedRNG(owner->id);
    // Build an expression based on the achiever's stats and evaluate it to get the damage.
    const auto& stats = owner->achiever()->extra_stats;
    const auto operations = {"+", "+", "+", "*"};
    float dmg = 1.0f;

    // iterate over the stats
    for (const auto& [name, stat] : stats)
    {
        std::vector<const char*> sampled_operation;
        std::sample(operations.begin(), operations.end(), std::back_inserter(sampled_operation), 1,
                    RandomGenerator());
        const auto op = sampled_operation.front();
        const float stat_value = stat->value;

        switch (op[0])
        {
        case '+':
            dmg += stat_value;
            break;
        /* case '-': */
        /*     dmg -= stat_value; */
        /*     break; */
        case '*':
            dmg *= stat_value;
            break;
        /* case '/': */
        /*     dmg /= (stat_value + 1); // Avoid division by zero */
        /*     break; */
        default:
            dmg += stat_value;
            break;
        }
    }
    Log("Calculated damage for " << owner->name << ": " << dmg);
    return std::max(1, static_cast<int>(dmg));
}
