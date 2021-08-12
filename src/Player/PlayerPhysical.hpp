#ifndef PLAYER_PHYSICAL_HPP
#define PLAYER_PHYSICAL_HPP

#include "../Thing.hpp"
#include "../Room.hpp"
#include <memory>

class PlayerNetworked;

class PlayerPhysical : public Physical
{
public:

   PlayerPhysical()
    {
    }

   void doUpdate( const std::shared_ptr<Thing> &owner, World& world) override;
   void doMove(std::shared_ptr<Thing> owner, int x, int y) override;

   void moveDirection(std::shared_ptr<Thing> owner, const std::string& direction);
};

#endif
