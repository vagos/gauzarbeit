#ifndef LETTER_GAME_HPP
#define LETTER_GAME_HPP

#include "Widget.hpp"
#include <stdlib.h>

struct FloatingLetter : public Widget
{
    
    FloatingLetter(char c, int x, int y): Widget(x, y, 1, 1) {}
    
    char c;
    int x;
    int y;
};


class LetterGame : public Widget
{
    
    

    void doUpdate(Graphics &gfx) override
    {
        fFloatingLetterTimer -= gfx.getElapsedTime();
        
        if (fFloatingLetterTimer > 0) return;

        fFloatingLetterTimer = fFloatingLetterCooldown; // Reset the timer.

        FloatingLetter newLetter('a', rand() % 100, rand() % 100);

        vLetters.push_back(newLetter);
         
    }
    
    void doRender(Graphics &gfx) override
    {

    
    }

    private:
        std::vector<FloatingLetter> vLetters;

        float fFloatingLetterCooldown = 0.5f;
        float fFloatingLetterTimer = 0.0f;
};


#endif//LETTER_GAME_HPP
