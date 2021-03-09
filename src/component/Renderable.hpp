#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include "../Graphics.hpp"

// Thing can be rendered on the screen.
class Thing;

class Renderable  
{

    public: 
        Renderable();
        Renderable(int sprite_x, int sprite_y);
    
    virtual void doUpdate(Graphics& gfx, Thing& thing);
    virtual void doUpdate(Graphics& gfx, int x, int y); // I looove being a pointer. 

    static void loadTexture(const std::string filename) {textureTiles.loadFromFile(filename);} 

    void setTexture (sf::Texture& texture);
    void setSpritePos(int x, int y);
   
    void setTextureOffset(int sprite_x_, int sprite_y_);
    
    private:
        int sprite_x, sprite_y;
        static const int nSpriteSize = 8;
        sf::Sprite sprite;
        static sf::Texture textureTiles;

};

#endif /* RENDERABLE_HPP */
