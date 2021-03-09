#include "Border.hpp"
#include "Widget.hpp"



Border::Border(int x, int y, int w, int h): Widget(x, y, w, h),
    borderRect(sf::Vector2f(w, h))
{
    borderRect.setPosition(x, y);
}

void Border::doRender(Graphics &gfx)
{
    gfx.getWindow().draw(borderRect);
}

void Border::doMove(int x_, int y_)
{
    borderRect.setPosition(x_ + x, y_ + y);
}

