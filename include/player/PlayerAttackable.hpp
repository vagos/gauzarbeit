#ifndef PLAYER_ATTACKABLE_HPP
#define PLAYER_ATTACKABLE_HPP

#include "Room.hpp"
#include "thing/Attackable.hpp"

class PlayerAttackable : public Attackable
{
  public:
	PlayerAttackable();

	void doUpdate(const std::shared_ptr<Thing>& owner) override;
	void doAttack(const std::shared_ptr<Thing>& owner,
				  const std::shared_ptr<Thing>& target) override;
	void onAttack(const std::shared_ptr<Thing>& owner,
				  const std::shared_ptr<Thing>& attacker) override;
	void onDeath(const std::shared_ptr<Thing>& owner) override;
	void doRespawn(const std::shared_ptr<Thing> owner);
};

#endif
