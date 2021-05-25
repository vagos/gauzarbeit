#ifndef QUEST_HPP
#define QUEST_HPP

#include <memory>
#include <string>

#include "Physical.hpp"
#include "Thing.hpp"




class Quest : public Thing
{
public:

    Quest()
    {
        tasker = std::make_unique<Tasker>();
    }

};


#endif//QUEST_HPP
