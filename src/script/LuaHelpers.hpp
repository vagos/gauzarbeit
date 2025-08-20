#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP

#include <algorithm>
#include <iostream>
#include <lua.hpp>
#include <memory>
#include <string>

class Thing;

bool CheckLua(lua_State* L, int r);

template <typename T> const std::shared_ptr<Thing> GetSmartPtr(const T& c, Thing* t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(),
                          [&t_ptr](const std::shared_ptr<Thing>& t) { return t.get() == t_ptr; });

    return r != c.end() ? *r : nullptr;
}

#endif
