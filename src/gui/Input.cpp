#include "iostream"

#include "Input.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

Input::Input(int x, int y, int max_c):
    Label(x, y, sf::Color::Black) 
{
    startTyping();
}


void Input::doUpdate(Graphics& gfx)
{
    if (keyboardFocus != this) return;

    for (const auto& event : gfx.getEvents() )
    {
        if (event.type == sf::Event::TextEntered) 
        {
            switch(event.text.unicode)
            {
            
               case '\b':
                   if ( sInput.getSize() ) sInput.erase(sInput.getSize() - 1); 
                   break;
               case '\r':
                   stopTyping();
                   break;
               default: 
                   sInput += event.text.unicode;
                   break;
            
            }   
        }
        
    }
       
    textLabel.setString(sInput);
}

void Input::doRender(Graphics& gfx) 
{
       gfx.getWindow().draw(textLabel);

}
