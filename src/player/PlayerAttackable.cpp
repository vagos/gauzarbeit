#include "player/PlayerAttackable.hpp"

PlayerAttackable::PlayerAttackable()
{
	current_health = 15;
	max_health = 20;
}

void PlayerAttackable::doUpdate(const std::shared_ptr<Thing>& owner)
{
	if (!alive)
	{
		doRespawn(owner);
	}
}

void PlayerAttackable::doAttack(const std::shared_ptr<Thing>& owner,
								const std::shared_ptr<Thing>& target)
{
	std::stringstream res;

	res << "You attacked " << target->name << '\n'; // TODO Put this into notifier.

	owner->networked()->addResponse(res.str());

	Attackable::doAttack(owner, target);
}

void PlayerAttackable::onAttack(const std::shared_ptr<Thing>& owner,
								const std::shared_ptr<Thing>& attacker)
{
	std::stringstream res;

	res << attacker->name << " attacked you!\n";

	owner->networked()->addResponse(res.str());

	Attackable::onAttack(owner, attacker);
}

void PlayerAttackable::onDeath(const std::shared_ptr<Thing>& owner)
{
	alive = false;

	owner->networked()->addResponse(ColorString("You died!\n", Color::Red));

	owner->notifier()->doNotify(owner, Event::Type::Death); // notify everyone about the death
}

void PlayerAttackable::doRespawn(const std::shared_ptr<Thing> owner)
{
	alive = true;

	owner->physical()->doMove(owner, 0, 0);

	current_health = max_health;

	owner->networked()->addResponse("You respawned!\n");
}
