#ifndef LIST_HPP
#define LIST_HPP

#include "Container.hpp"
#include <SFML/System/String.hpp>

class List : public Container
{
    List(int x, int y, int w, int h, std::vector<sf::String>);


    int getSelected() {return nSelected;}

    void doUpdate(Graphics &gfx) override;
    
    
    private:
        int nSelected;
        std::vector<sf::String> vOptions;



};



#endif//LIST_HPP
