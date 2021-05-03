#include "Networked.hpp"
#include "World.hpp"
#include <bits/c++config.h>


void Networked::sendMessages(std::shared_ptr<Thing> owner, const World &world)
{
        while (! qMessages.empty()) 
        {
            auto messagePair = qMessages.front();

            if (! messagePair.second.empty()) // If the message has a recipient
            {
               auto p = world.getPlayer(messagePair.second);

               p -> networked -> addResponse(messagePair.first);
            }

            else 
            {
                for (auto& [name, player] : world.playersOnline)
                {
                    player -> networked -> addResponse(messagePair.first);
                }

            }

            qMessages.pop();

        }

}

std::size_t Networked::lastID = 0;
