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
        // currentRoom = Room::get(0, 0); 

        // currentRoom -> addPlayer( owner -> shared_from_this() );

    }

   void doUpdate( std::shared_ptr<Thing> owner, World& world) override;
    void doMove(std::shared_ptr<Thing> owner, int x, int y) override;

private:
   void moveDirection(std::shared_ptr<Thing> owner, const std::string& direction);
};

#endif
