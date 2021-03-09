#include "Widget.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <string.h>
#include <list>

#include <iostream>


Widget* Widget::focus = nullptr;
Widget* Widget::keyboardFocus = nullptr;
std::list<Widget*> Widget::listWidgets;

Widget::Widget() : x(0), y(0), w(0), h(0)
{
  listWidgets.push_back(this);
}

Widget::Widget(int x, int y) : x(x), y(y), w(0), h(0)
{
  listWidgets.push_back(this);
}

Widget::Widget(int x, int y, int w, int h)
    : x(x), y(y), 
    w(w), h(h)
{
  listWidgets.push_back(this);
}

Widget::~Widget() 
{
  if (focus == this) focus = NULL;
  listWidgets.remove(this);
}

void Widget::setBackgroundColor(const sf::Color col, const sf::Color colFocus) 
{
  colorBack = col;
  colorBackFocus = colFocus;
}

void Widget::setForegroundColor(const sf::Color col, const sf::Color colFocus) 
{
  colorFore = col;
  colorForeFocus = colFocus;
}

void Widget::_doUpdate(Graphics &gfx) 
{
     
     for (const auto& event : gfx.getEvents())
     {
        
         if (event.type == sf::Event::MouseMoved)
         {
            bool isInside = is_inside(event.mouseMove.x, event.mouseMove.y); 
         
             
        
             if (isInside && !bMouseInside) // Check for mouse placement relative to the widget.
             {
                 onMouseIn();
                 bMouseInside = true;
             }

             else if (!isInside && bMouseInside)
             {
                 onMouseOut();
                 bMouseInside = false;
             }
         } 
         
         if (!bMouseInside) goto UPDATE; // Evil goto is NOT evil.
          
         if ( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left ) // Check for mouse interaction with the widget.
         {
             onButtonPress();
         } 
         else if ( event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left )
         {
             onButtonRelease();
             onButtonClick();
         }
         
     }

    UPDATE:
        doUpdate(gfx); // Might change this.

}


bool Widget::isVisible()
{
    if (wParent) return wParent->isVisible();

    return bVisible;
}

void Widget::_doRender(Graphics& gfx)
{
    doRender(gfx);
}

void Widget::updateWidgets(Graphics& gfx) 
{

  for (const auto& w : listWidgets) 
  {
      (w) ->  _doUpdate(gfx);
  }

}

void Widget::renderWidgets(Graphics& gfx) {
  
    for (const auto& w : listWidgets) 
  {
        if ( (w) -> isVisible() )  (w) ->  _doRender(gfx);
  }
}

void Widget::move(int x_, int y_) 
{
  this->x = x_;
  this->y = y_;
}

