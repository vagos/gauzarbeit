#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Renderable.hpp"
#include "../Graphics.hpp"
#include "../Thing.hpp"

sf::Texture Renderable::textureTiles = sf::Texture();


Renderable::Renderable() 
{

}

Renderable::Renderable(int sprite_x, int sprite_y) 
{
    setTextureOffset(sprite_x, sprite_y);
    sprite.setTexture(textureTiles);
}


void Renderable::doUpdate(Graphics& gfx, Thing &thing)
{
    int x = thing.getPhysical().x;
    int y = thing.getPhysical().y;

    setSpritePos(x, y);

    gfx.getWindow().draw(sprite);

}

void Renderable::setSpritePos(int x, int y) 
{
    sprite.setPosition(x * nSpriteSize, y * nSpriteSize);
}


void Renderable::doUpdate(Graphics& gfx, int x, int y)
{
   setSpritePos(x, y); 
   gfx.getWindow().draw(sprite);
}

void Renderable::setTexture(sf::Texture &texture)
{
    sprite.setTexture(texture);
}


void Renderable::setTextureOffset(int sprite_x_, int sprite_y_)
{
     sprite_x = sprite_x_; 
     sprite_y = sprite_y_;

     sprite.setTextureRect(sf::IntRect(sprite_x * nSpriteSize, sprite_y * nSpriteSize, 
                nSpriteSize, nSpriteSize));


}
