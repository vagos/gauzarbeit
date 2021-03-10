#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Graphics.hpp"
#include "gui/List.hpp"
#include "component/Renderable.hpp"
#include "Thing.hpp"
#include "World.hpp"
#include "gui/Input.hpp"
#include "gui/Widget.hpp"
#include "gui/CharacterMenu.hpp"

int main()
{
    // Graphics...
    Graphics gfx;

    gfx.getWindow().setFramerateLimit(60);

    sf::RenderWindow& win = gfx.getWindow();
    
        
    // Load the Tile Texture for all Renderables.
    Renderable::loadTexture("../media/terminal.png"); 
    Graphics::loadFont("../media/fonts/AcPlus_IBM_BIOS.ttf");
    
    sf::Text text;
    text.setFont(Graphics::mainFont);

    text.setString("Welcome to Gauzarbeit"); 

    //text.setPosition(5, 5);
    text.setCharacterSize(9);
    text.setFillColor(sf::Color::White);


    Wall wall;
    Player player;
    World world(30, 30);
    

    //Input myInput(10, 10, 30);

    List myList (10, 10, 100, 100, {"Option1", "Option2", "Option3"});

    Widget::keyboardFocus = (Widget *)&myList;

    CharacterMenu charMenu(player);
    
    
    // Start the game loop
    
    while (gfx.getWindow().isOpen())
    {
       
        // Clear screen   
        gfx.getWindow().clear();
        
        sf::Event event;
        
        while ( gfx.getWindow().pollEvent(event) ) 
        {
            switch (event. type)
            {
                case  sf::Event::Closed :
                    gfx.getWindow().close();
                    break;

                case  sf::Event::KeyReleased :
                    break;

                case  sf::Event::Resized:
                    {
                            // update the view to the new size of the window
                            sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                            gfx.getWindow().setView( sf::View(visibleArea));
                    }

                default:
                    gfx.addEvent(event);
            }
        }
         
         
        //world.doUpdate(gfx);
        player.doUpdate(gfx);
        
        Widget::updateWidgets(gfx);
        Widget::renderWidgets(gfx);
        
        gfx.getWindow().display();
        gfx.doUpdate();


    }

    return EXIT_SUCCESS;
}
