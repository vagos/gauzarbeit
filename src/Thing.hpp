#ifndef THING_HPP
#define THING_HPP

#include "component/Renderable.hpp"
#include "Graphics.hpp"
//This is the base class for every Game object.


// Thing acts on it's own.

class Intelligent
{
    virtual void doUpdate() {};

    private:
        float fUpdateTime; //How often the object's AI is run.
};


// Thing can die / get destroyed.

class Destructible
{
    
    virtual void doDie() {}

};

// The player / other things can use the thing.

class Usable
{
    virtual void doUse() {};

};

class Physical
{
    public:
        Physical(bool bSolid): bSolid(bSolid), x(0), y(0)
        {
        }
        
        Physical (int x, int y): x(x), y(y)
        {
    
        }
        
        bool isSolid() { return bSolid; }

        void setPos(int x_, int y_) {x = x_; y = y_;}
    
    bool bSolid;
    int x, y;

    sf::Vector2f vPos;
    sf::Vector2f vVel;

};

class Thing
{

    protected:
        Renderable   * renderable   = nullptr;    
        Intelligent  * intelligent  = nullptr;   
        Destructible * destructible = nullptr;   
        Usable       * usable       = nullptr;    
        Physical     * physical     = nullptr;

    public:
        
        Renderable     & getRenderable()   { return *renderable;   }
        Intelligent    & getIntelligent()  { return *intelligent;  }
        Destructible   & getDestructible() { return *destructible; }
        Usable         & getUsable()       { return *usable;       }
        Physical       & getPhysical()     { return *physical;     } 

    virtual void doUpdate() {}
    virtual void doUpdate(Graphics& gfx) {}

};

// A simple wall that does nothing.

class Wall : public Thing 
{
    public:
    Wall() 
    {
        renderable = new Renderable(0, 0);
        physical = new Physical(true);
    }

    void doUpdate(Graphics& gfx) 
    {
       getRenderable().doUpdate(gfx, *this); 
    }
};

class Air : public Thing
{
    public:
    
        Air() : Thing()
        {
            //renderable = new Renderable(' ');
            physical = new Physical(false);
            
        }
    

};

#endif /* THING_HPP */
