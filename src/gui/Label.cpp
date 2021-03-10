#include "Label.hpp"
#include <iostream>

Label::Label(int x, int y, const sf::String& sText, sf::Color color): Widget(x, y, sText.getSize(), 1)
{

   textLabel.setString(sText);
   textLabel.setCharacterSize(Graphics::nCharSize);
   textLabel.setFillColor(color);
   textLabel.setFont(Graphics::mainFont);
    
   textLabel.setPosition(x, y);
}

Label::Label(int x, int y, sf::Color color): Widget(x, y, 10, 1)
{
    textLabel.setCharacterSize(Graphics::nCharSize);
    textLabel.setFillColor(color);
    textLabel.setFont(Graphics::mainFont);

    textLabel.setPosition(x, y);
}


void Label::doRender(Graphics& gfx) 
{
    if (focus == this) textLabel.setFillColor(colorForeFocus);
    else textLabel.setFillColor(colorFore);

    gfx.getWindow().draw(textLabel);

}


void Label::doMove(int x_, int y_)
{
    textLabel.setPosition(x + x_, y + y_);
}



