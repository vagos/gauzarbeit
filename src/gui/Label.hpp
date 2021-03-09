// A simple label that holds a piece of text.
#ifndef LABEL_HPP
#define LABEL_HPP

#include "Widget.hpp"
#include <SFML/Graphics/Color.hpp>

class Label : public Widget
{
   
    public: 

        Label(int x, int y, const sf::String& sText, sf::Color color = sf::Color::White);
        Label(int x, int y, sf::Color color);

        void doRender(Graphics &gfx) override;  
        void doMove(int x_, int y_) override;
    
    protected:
        sf::Text textLabel;

};


#endif /* LABEL_HPP */
