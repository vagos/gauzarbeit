#ifndef SCRIPTED_THING_HPP
#define SCRIPTED_THING_HPP

#include "../Thing.hpp"

class lua_State;

class ScriptedThing : public Thing
{

public: 
    ScriptedThing(const std::string& name);

    const std::string getInfo() const override
    {
        return "[Scripted]";
    }


    static lua_State * L;

};





#endif
