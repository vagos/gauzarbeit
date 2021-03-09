#include "Container.hpp"
#include <SFML/Window/Event.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <iostream>

Container::Container(int x, int y, int w, int h): Widget(x, y, w, h)
{
}


void Container::addWidget(Widget *child)
{
    
    child->doMove(x, y); // Set the child's position relative to the container.
    child->wParent = this;
    vChildren.push_back(child);
    
    listWidgets.remove(child);
    
}


void Container::_doUpdate(Graphics& gfx)
{
    Widget::_doUpdate(gfx);

    for (const auto& c : vChildren)
    {
        c->_doUpdate(gfx);
    }
    
    
    if (!bDragging) return;

    for (const auto& event : gfx.getEvents()) // Drag the Container around.
    {
        if (event.type == sf::Event::MouseMoved)
        {
            doMove(event.mouseMove.x - w/2, event.mouseMove.y - h/2); 
        }
        
    }
    
}



void Container::_doRender(Graphics &gfx)
{
    
    for (const auto& c : vChildren)
    {
        c->_doRender(gfx);
    }
 

}

void Container::doMove(int x_, int y_)
{
   
   setPos(x_, y_);  
 
   for (const auto& c : vChildren)
   {
        c->doMove(x, y);
   }
       
}
