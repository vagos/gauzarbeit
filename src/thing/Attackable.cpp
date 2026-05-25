#include "thing/Attackable.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"
#include "Helpers.hpp"
#include <cassert>
#include <iterator>
#include <vector>

void Attackable::doAttack(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& target)
{
    assert(owner->_physical && target->_physical);
    assert(owner->physical()->current_room == target->physical()->current_room);
    assert(owner->_notifier && target->_notifier);
    Log(owner->name << " is attacking " << target->name);

    owner->notifier()->doNotify(owner, Event::Type::Attack, target);
    target->notifier()->clearEvent();
    target->notifier()->event.type = Event::Type::Attacked;
    target->notifier()->event.verb = "attacked";
    target->notifier()->event.target = owner->name;
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
    // Notify everyone about the death
    owner->notifier()->doNotify(owner, Event::Type::Death);

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
