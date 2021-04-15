#ifndef PLAYER_PHYSICAL_HPP
#define PLAYER_PHYSICAL_HPP

#include "../Thing.hpp"
#include "../Room.hpp"

class PlayerNetworked;

class PlayerPhysical : public Physical
{
public:

   PlayerPhysical(std::shared_ptr<Thing> owner): Physical(owner) 
    {
        currentRoom = Room::get(0, 0); 

        currentRoom -> addPlayer( owner -> shared_from_this() );

    }

   void doUpdate() override;


private:

   void moveDirection(const std::string& direction, const std::shared_ptr<PlayerNetworked>& net);
};

#endif
