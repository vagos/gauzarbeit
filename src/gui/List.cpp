#include "List.hpp"
#include "Label.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>


List::List(int x, int y, int w, int h, std::vector<sf::String> vOptionStrings) : Container(x, y, w ,h) 
{
    
    for (int i = 0; i < vOptionStrings.size(); i++)
    {
        auto newLabel = new Label(x, y + 10*i, vOptionStrings[i], sf::Color::White);

        addWidget(newLabel);

        vOptions.push_back(newLabel);

        
    }
    
    vOptions[0] -> doFocus();

}


void List::doUpdate(Graphics &gfx)
{
    

}


void List::getKeyboardInput(Graphics& gfx)
{
    
    for (const auto& event : gfx.getEvents())
    {
        if (event.type == sf::Event::KeyPressed)
        {

            vOptions[nSelected] -> doUnfocus(); 
            
            switch(event.key.code)
            {
                case sf::Keyboard::Up:
                    nSelected = (nSelected - 1) % vOptions.size();
                    break;
                
                case sf::Keyboard::Down:
                    nSelected = (nSelected + 1) % vOptions.size();
                    break;
                case '\r':
                    //doSelect();
                    break;
            }
            
            vOptions[nSelected]->doFocus();
        
        }
    }

}
