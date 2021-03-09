#include "List.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>


List::List(int x, int y, int w, int h, std::vector<sf::String> vOptions) : Container(x, y, w ,h), vOptions(vOptions)
{
    
    for (const auto& s : vOptions)
    {
        
    }


}


void List::doUpdate(Graphics &gfx)
{
    for (const auto& event : gfx.getEvents())
    {
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
                case sf::Keyboard::Up:
                    nSelected = (nSelected - 1) % vOptions.size();
            }
        
        }
    }

}
