#ifndef TEST_ITEM_HPP
#define TEST_ITEM_HPP



#include "Thing.hpp"


class TestUsable : public Usable
{

public:
    void doUse(std::shared_ptr<Thing> user) override
    {
        
        if ( user -> networked ) 
        {


        }

    }

};

class TestItem : public Thing
{
public: 
    TestItem()
    {
        usable = std::make_shared<TestUsable>();
    }

};


#endif
