#include "PlayerPhysical.hpp"
#include "PlayerNetworked.hpp"

void PlayerPhysical::doUpdate()
{

    std::shared_ptr<PlayerNetworked> net = std::static_pointer_cast<PlayerNetworked>(owner -> networked); 
        
    std::stringstream requestStream { net->getRequestStream().str() };

    std::string sVerb; 

    requestStream >> sVerb;

    if (sVerb == "move")
    {
        std::string sDirection;

        requestStream >> sDirection;

        moveDirection(sDirection, net);

    }


}



void PlayerPhysical::moveDirection(const std::string& direction, const std::shared_ptr<PlayerNetworked>& net)
{

        if (direction == "left")
        {
            doMove(currentRoom -> x - 1, currentRoom -> y);
        }
        
        else if (direction == "right")
        {
            doMove(currentRoom -> x + 1, currentRoom -> y);
        }
   
        else if (direction == "down")
        {
            doMove(currentRoom -> x, currentRoom -> y + 1);
        }
   
        else if (direction == "up")
        {
            doMove(currentRoom -> x, currentRoom -> y - 1);
        }

        else return;

        std::stringstream res; 

        res << "Moved into Room: " << currentRoom -> x << " " << currentRoom -> y << "\n";

        net -> addResponse( res.str() );

}
