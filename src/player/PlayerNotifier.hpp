#ifndef PLAYER_NOTIFIER_HPP
#define PLAYER_NOTIFIER_HPP

#include "Room.hpp"
#include "thing/Thing.hpp"
#include <cassert>
#include <iomanip>
#include <sstream>

class PlayerNotifier : public Notifier
{

    void doNotify(const std::shared_ptr<Thing>& owner, const Event& event,
                  const std::shared_ptr<Thing>& target = nullptr) override;
    void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                  const Event& event, const std::shared_ptr<Thing>& target) override;
};

#endif
