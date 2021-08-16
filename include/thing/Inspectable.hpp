#ifndef INSPECTABLE_HPP
#define INSPECTABLE_HPP

#include <string>
#include <memory>

class Thing;

class Inspectable
{
public:
    Inspectable();

    virtual const std::string onInspect(const std::shared_ptr<Thing> &owner, const std::shared_ptr<Thing> &inspector);
    virtual const std::string onHelp(std::shared_ptr<Thing> owner, std::shared_ptr<Thing> inspector);
    virtual const std::string getName(const std::shared_ptr<Thing> &owner);
};

#endif
