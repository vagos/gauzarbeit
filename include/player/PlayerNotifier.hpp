#ifndef PLAYER_NOTIFIER_HPP
#define PLAYER_NOTIFIER_HPP

#include "Quest.hpp"
#include "Room.hpp"
#include "thing/Thing.hpp"

#include <cassert>
#include <iomanip>
#include <sstream>

class PlayerNotifier : public Notifier
{

	void doNotify(const std::shared_ptr<Thing>& owner, Event::Type notification_type,
				  const std::shared_ptr<Thing>& target = nullptr) override;
	void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
				  Event::Type notification_type, const std::shared_ptr<Thing>& target) override;

	void setEvent(const std::shared_ptr<Thing>& owner) override;
};

#endif
