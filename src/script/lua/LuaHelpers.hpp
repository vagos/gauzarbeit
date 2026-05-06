#ifndef LUA_HELPERS_HPP
#define LUA_HELPERS_HPP

#include <algorithm>
#include <iostream>
#include <lua.hpp>
#include <memory>
#include <stdexcept>
#include <string>

class Thing;

#define CheckLua(L, r)                                                                             \
    do                                                                                             \
    {                                                                                              \
        if ((r) != LUA_OK)                                                                         \
        {                                                                                          \
            std::string errormsg = lua_tostring((L), -1);                                          \
            Log(errormsg);                                                                         \
            throw std::runtime_error("Lua error: " + errormsg);                                    \
        }                                                                                          \
    } while (0)

template <typename T> const std::shared_ptr<Thing> GetSmartPtr(const T& c, Thing* t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(),
                          [&t_ptr](const std::shared_ptr<Thing>& t) { return t.get() == t_ptr; });

    return r != c.end() ? *r : nullptr;
}

#endif
