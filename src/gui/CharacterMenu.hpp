#ifndef CHARACTER_MENU_HPP
#define CHARACTER_MENU_HPP

#include "Container.hpp"
#include "Label.hpp"
#include "Border.hpp"
#include "Input.hpp"
#include "../Player.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

class CharacterMenu : public Container
{
    public:
    CharacterMenu(Player& player_): Container(100, 0, 300, 300), player(player_)
    {
        addWidget(new Border(0, 0, w, h));
        addWidget(new Label(0, 0, player.sName, sf::Color::Blue));
        addWidget(new Input(0, 20, 10));
    }

    void doUpdate(Graphics &gfx) override
    {
       for (const auto& event : gfx.getEvents())
       {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::I)
                toggleVisible();
       }
        
    }

    private:
        Player& player;
};

#endif
