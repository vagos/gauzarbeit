#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "Widget.hpp"
#include <iostream>

class Container : public Widget
{
    public:
        Container(int x, int y, int w, int h); 

        ~Container() {vChildren.clear();}
    
    public:
        void addWidget(Widget * child);
        void doUpdate(Graphics &gfx) override {};
    
    private:

        void onButtonPress() override
        {
            bDragging = true;
        }

        void onButtonRelease() override
        {
            bDragging = false;
        }

        void doMove(int x_, int y_) override;
        
        void _doRender(Graphics &gfx) override;
        void _doUpdate(Graphics &gfx) override;

    private:
        std::vector<Widget*> vChildren;
        bool bDragging = false; // Whether the user is dragging this widget.


};

#endif // CONTAINER_HPP 
