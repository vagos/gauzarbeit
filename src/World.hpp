#ifndef WORLD_HPP
#define WORLD_HPP

#include"Thing.hpp"

class World 
{

public:

    World(int w, int h);
    
    void doUpdate(Graphics& graphics);
    Thing * getTile(int x, int y) const { return tTiles[x + y * w];}

private:
    int w, h;    

    Thing ** tTiles;
    
    static const Thing * tWall;
    static const Thing * tAir; 

};

#endif /* WORLD_HPP */
