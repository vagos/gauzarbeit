#include "Player.hpp"
#include "component/Renderable.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "Graphics.hpp"

Player::Player ( ): sName("PlayerName")
{
    renderable = new Renderable(4, 0);   
    physical = new Physical(true);
}


void Player::doUpdate(Graphics &gfx)
{
    fControlTimer -= gfx.getElapsedTime(); 

    for (const auto& event :  gfx.getEvents())
    {
        if (event.type == sf::Event::KeyPressed)    
            getInput(event);
    
    }
    
    getRenderable().doUpdate(gfx, *this);

}


void Player::getInput(const sf::Event& event)
{

    switch ( event.key.code )
    {
        case sf::Keyboard::Right:
                getPhysical().x += 1;
                break;
        case sf::Keyboard::Left:
                getPhysical().x -= 1;
                break;
        case sf::Keyboard::Up:
                getPhysical().y -= 1;
                break;
        case sf::Keyboard::Down:
                getPhysical().y += 1;
                break;

        default:
            break;
    }

}
