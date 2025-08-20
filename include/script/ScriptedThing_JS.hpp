#pragma once

#include "thing/Thing.hpp"
#include <quickjs.h>

#define CheckJS(ctx, val)                                                                          \
    do                                                                                             \
    {                                                                                              \
        if (JS_IsException(val))                                                                   \
        {                                                                                          \
            JSValue exception = JS_GetException(ctx);                                              \
            const char* error_msg = JS_ToCString(ctx, exception);                                  \
            Log("JavaScript error: " << error_msg << std::endl);                                   \
            JS_FreeCString(ctx, error_msg);                                                        \
            JS_FreeValue(ctx, exception);                                                          \
        }                                                                                          \
    } while (0)

// Native C++ function we want to expose to JS
static JSValue print(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
    if (argc < 1 || !JS_IsString(argv[0]))
    {
        Log("print: Expected a string argument." << std::endl);
        return JS_EXCEPTION;
    }

    const char* str = JS_ToCString(ctx, argv[0]);
    if (!str)
    {
        Log("print: Failed to convert argument to string." << std::endl);
        return JS_EXCEPTION;
    }

    std::clog << str << std::endl;
    JS_FreeCString(ctx, str);
    return JS_UNDEFINED;
}

class ScriptedThing_JS : Thing
{
  public:
    ScriptedThing_JS(const std::string& name, const std::string& script_dir = "./scripts/things/")
        : Thing(name)
    {

        std::string filename(script_dir + name + ".js");

        JSValue global = JS_GetGlobalObject(ctx);

        std::ifstream file(filename, std::ios::binary);
        std::string code =
            std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        JSValue proto = JS_GetPropertyStr(ctx, global, "__THING_PROTO__");
        JSValue obj = JS_NewObjectProtoClass(ctx, proto, classID);
        JS_SetOpaque(obj, this);
        // Expose on globalThis[name]
        JS_DefinePropertyValueStr(ctx, global, this->name.c_str(), JS_DupValue(ctx, obj),
                                  JS_PROP_C_W_E);

        JSValue val =
            JS_Eval(ctx, code.c_str(), code.size(), filename.c_str(), JS_EVAL_TYPE_GLOBAL);
        CheckJS(ctx, val);

        JSValue fn = JS_GetPropertyStr(ctx, obj, "doInit");
        assert(JS_IsFunction(ctx, fn));
        CheckJS(ctx, fn);
        JSValue ret = JS_Call(ctx, fn, obj, 1, &obj);

        JS_FreeValue(ctx, fn);
        JS_FreeValue(ctx, ret);
        JS_FreeValue(ctx, global);
        JS_FreeValue(ctx, proto);
    }

    static Thing* self(JSContext* ctx, JSValueConst this_val)
    {
        return static_cast<Thing*>(JS_GetOpaque2(ctx, this_val, classID));
    }

    static JSValue getName(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        JSValue t_n = JS_NewString(ctx, t->name.c_str());
        return t_n;
    }

    static void Init()
    {
        std::string filename("scripts/Init.js");
        std::ifstream file(filename, std::ios::binary);
        std::string code =
            std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        JSValue global = JS_GetGlobalObject(ctx);

        // Register 'print' function
        JS_SetPropertyStr(ctx, global, "print", JS_NewCFunction(ctx, print, "print", 1));

        JS_NewClassID(&classID);
        JSClassDef def{};
        def.class_name = "Thing";
        def.finalizer = [](JSRuntime*, JSValue val)
        {
            // no-op; C++ owns Thing*
            (void)val;
        };
        JS_NewClass(rt, classID, &def);

        JSValue val =
            JS_Eval(ctx, code.c_str(), code.size(), filename.c_str(), JS_EVAL_TYPE_GLOBAL);
        CheckJS(ctx, val);

        JSValue proto = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, proto, "getName",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getName, "getName", 0));
        // TODO: Add the rest...

        JS_DefinePropertyValueStr(ctx, global, "__THING_PROTO__", JS_DupValue(ctx, proto),
                                  JS_PROP_C_W_E);
        JS_SetClassProto(ctx, classID, proto);

        JS_FreeValue(ctx, proto);
        JS_FreeValue(ctx, global);
    }

    static void DeInit()
    {
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
    }

    static JSRuntime* rt;
    static JSContext* ctx;
    static JSClassID classID;
};
