#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "Graphics.hpp"
#include "Thing.hpp"
#include <SFML/Window/Event.hpp>

class Player : public Thing
{
public:
    Player();
    
    void doUpdate(Graphics& gfx);

    const sf::String sName;

private:
    void getInput(const sf::Event& event);
    float fControlTimer = 0.0f; // Time between controls are registered.
    const float fControlCooldown = 0.0;


};

#endif /* PLAYER_HPP */ 
