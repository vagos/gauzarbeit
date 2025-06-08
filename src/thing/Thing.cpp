#include "thing/Thing.hpp"
#include "Room.hpp"

std::vector<std::shared_ptr<Thing>> Thing::things; // TODO: Use this

Thing::Thing() : name("Thing") {}

Thing::Thing(const std::string& name) : name(name) {}

void Thing::doUpdate(World& world)
{
	if (_physical)
		_physical->doUpdate(shared_from_this(), world);
	if (_usable)
		_usable->doUpdate(shared_from_this());
	if (_attackable)
		_attackable->doUpdate(shared_from_this());
	if (_thinker)
		_thinker->doThink(shared_from_this());
	if (_notifier)
		_notifier->doUpdate(shared_from_this());
}
