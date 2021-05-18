#ifndef SCRIPTED_THING_HPP
#define SCRIPTED_THING_HPP

#include "../Thing.hpp"

class lua_State;

class ScriptedThing : public Thing
{

public: 
    ScriptedThing(const std::string& name);

    ~ScriptedThing()
    {
        std::clog << sName << " got deleted!\n";
    }

    const std::string getInfo() const override
    {
        return "[Scripted]";
    }

    static int Index(lua_State * L);
    
    static int NewIndex(lua_State * L);
    
    static int GetName(lua_State * L);

    static int LoseItem(lua_State * L);
    static int GetThing(lua_State * L);
    
    static int SendMessage(lua_State * L);
    static int SetMaxHealth(lua_State * L);
    
    static void InitLua();

    static lua_State * L;

    int luaRef;

    static const std::string scriptDir;

};

#endif
