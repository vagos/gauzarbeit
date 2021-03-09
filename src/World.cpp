#include "World.hpp"
#include "Graphics.hpp"



World::World (int w, int h): w(w), h(h)
{
    tTiles = new Thing* [ w * h ];


    for (int i = 0 ; i < w*h; i ++)
    {
        tTiles[i] = (Thing*)tWall;
    }
}

void World::doUpdate(Graphics& graphics)
{
    for (int x = 0; x < w; x ++)
        for (int y = 0; y < h; y ++)
        {
            getTile(x, y) -> getPhysical().setPos(x, y);
            getTile(x, y) -> doUpdate(graphics);
        }
}

const Thing * World::tWall = new Wall();
const Thing * World::tAir = new Air();
