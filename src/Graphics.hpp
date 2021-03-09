#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SFML/Graphics.hpp"
#include <SFML/Window/VideoMode.hpp>
#include <vector>

class Graphics
{

public:
    
    Graphics() : 
        window( sf::VideoMode (700, 700), "Gauzarbeit : New Age") 
    {
        
    }
    
    sf::RenderWindow&  getWindow() { return window; }
    
    void setElapsedTime(float fTime) { fElapsedTime = fTime; }
    float getElapsedTime() {return fElapsedTime;}

    void addEvent(sf::Event event) {vEvents.push_back(event);}

    static void loadFont(const std::string& filename) {mainFont.loadFromFile(filename); }

    const std::vector<sf::Event>& getEvents() {return vEvents;}

    void doUpdate() 
    {
        fElapsedTime = clock.getElapsedTime().asSeconds();
        clock.restart();
        
        window.display();
        vEvents.clear();
    }

public:

    static const int nCharSize = 9;
    static const int nSpriteSize = 9;

    static sf::Font mainFont;


private:
    float fElapsedTime;
    sf::RenderWindow window;
    sf::Clock clock;
    sf::View view;
    std::vector<sf::Event> vEvents;
};
#endif /* GRAPHICS_HPP */
