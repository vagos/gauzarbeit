#ifndef INPUT_HPP 
#define INPUT_HPP

#include "Label.hpp"
#include <SFML/System/String.hpp>

class Input : public Label
{

public:
    
    Input(int x, int y, int max_c);

    void doUpdate(Graphics &) override;
    void doRender(Graphics &) override;
    
    const sf::String& getInput() {return sInput;}
    
    void startTyping() {keyboardFocus = this;}
    void stopTyping() {keyboardFocus = nullptr;}


private:
    sf::String sInput; 
};

#endif /* INPUT_HPP */ 
