#ifndef BORDER_HPP
#define BORDER_HPP


#include "Widget.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
class Border : public Widget
{
public:
    Border(int x, int y, int w, int h);
    
    void doRender(Graphics &gfx) override; 
    void doMove(int x_, int y_) override;

private:
        sf::RectangleShape borderRect;

};

#endif
