#ifndef TALKER_HPP
#define TALKER_HPP

#include <memory>

class Thing;


class Talker // This component can give quests/dialog
{
public:

    Talker()
    {

    }

    virtual void doUpdate(const std::shared_ptr<Thing>& owner)
    {

    }

    virtual void onTalk(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing> talker);

};

#endif//TALKER_HPP
