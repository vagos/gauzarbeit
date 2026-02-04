#include "script/ScriptAPI.hpp"
#include "Helpers.hpp"
#include <cassert>
#include <iostream>
#include <lua.hpp>
#include <quickjs.h>

namespace
{
bool LuaHasField(lua_State* L, const char* field)
{
    lua_getfield(L, -1, field);
    bool ok = !lua_isnil(L, -1);
    lua_pop(L, 1);
    return ok;
}

bool JSHasField(JSContext* ctx, JSValueConst obj, const char* field)
{
    JSValue val = JS_GetPropertyStr(ctx, obj, field);
    bool ok = !JS_IsUndefined(val);
    JS_FreeValue(ctx, val);
    return ok;
}
} // namespace

void VerifyLuaAPI(lua_State* L)
{
    bool missing = false;

    luaL_getmetatable(L, "Gauzarbeit.Thing");
    for (const char* const* m = ScriptAPI::kThingMethods; *m; ++m)
    {
        if (!LuaHasField(L, *m))
        {
            Log("Lua API missing Thing method: " << *m);
            missing = true;
        }
    }
    lua_pop(L, 1);

    lua_getglobal(L, "Gauzarbeit");
    for (const char* const* f = ScriptAPI::kGauzarbeitFuncs; *f; ++f)
    {
        if (!LuaHasField(L, *f))
        {
            Log("Lua API missing Gauzarbeit function: " << *f);
            missing = true;
        }
    }

    lua_getfield(L, -1, "Event");
    for (const ScriptConstant* c = ScriptAPI::kEventConstants; c->name; ++c)
    {
        if (!LuaHasField(L, c->name))
        {
            Log("Lua API missing Gauzarbeit.Event constant: " << c->name);
            missing = true;
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "Color");
    for (const ScriptConstant* c = ScriptAPI::kColorConstants; c->name; ++c)
    {
        if (!LuaHasField(L, c->name))
        {
            Log("Lua API missing Gauzarbeit.Color constant: " << c->name);
            missing = true;
        }
    }
    lua_pop(L, 1);

    lua_pop(L, 1);

    assert(!missing);
}

void VerifyJSAPI(JSContext* ctx, JSValue proto)
{
    bool missing = false;

    for (const char* const* m = ScriptAPI::kThingMethods; *m; ++m)
    {
        if (!JSHasField(ctx, proto, *m))
        {
            Log("JS API missing Thing method: " << *m);
            missing = true;
        }
    }

    JSValue global = JS_GetGlobalObject(ctx);
    JSValue gauzarbeit = JS_GetPropertyStr(ctx, global, "Gauzarbeit");
    if (JS_IsUndefined(gauzarbeit))
    {
        Log("JS API missing Gauzarbeit global");
        missing = true;
    }
    else
    {
        for (const char* const* f = ScriptAPI::kGauzarbeitFuncs; *f; ++f)
        {
            if (!JSHasField(ctx, gauzarbeit, *f))
            {
                Log("JS API missing Gauzarbeit function: " << *f);
                missing = true;
            }
        }

        JSValue event_obj = JS_GetPropertyStr(ctx, gauzarbeit, "Event");
        for (const ScriptConstant* c = ScriptAPI::kEventConstants; c->name; ++c)
        {
            if (!JSHasField(ctx, event_obj, c->name))
            {
                Log("JS API missing Gauzarbeit.Event constant: " << c->name);
                missing = true;
            }
        }
        JS_FreeValue(ctx, event_obj);

        JSValue color_obj = JS_GetPropertyStr(ctx, gauzarbeit, "Color");
        for (const ScriptConstant* c = ScriptAPI::kColorConstants; c->name; ++c)
        {
            if (!JSHasField(ctx, color_obj, c->name))
            {
                Log("JS API missing Gauzarbeit.Color constant: " << c->name);
                missing = true;
            }
        }
        JS_FreeValue(ctx, color_obj);
    }

    JS_FreeValue(ctx, gauzarbeit);
    JS_FreeValue(ctx, global);

    assert(!missing);
}
