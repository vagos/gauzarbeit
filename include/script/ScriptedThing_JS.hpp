#pragma once

#include "Thing.hpp"
#include <quickjs.h>

#define CheckJS(ctx, val) \
    do { \
        if (JS_IsException(val)) { \
            JSValue exception = JS_GetException(ctx); \
            const char* error_msg = JS_ToCString(ctx, exception); \
            Log("JavaScript error: " << error_msg << std::endl); \
            JS_FreeCString(ctx, error_msg); \
            JS_FreeValue(ctx, exception); \
        } \
    } while (0)

// Native C++ function we want to expose to JS
static JSValue print(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
    if (argc < 1 || !JS_IsString(argv[0])) {
        return JS_EXCEPTION;
    }

    const char* str = JS_ToCString(ctx, argv[0]);
    if (!str) {
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

        // Register our native function as 'sayHello'
        JS_SetPropertyStr(ctx, global, "print",
                          JS_NewCFunction(ctx, print, "print", 1) // 1 arg
        );

        JS_FreeValue(ctx, global);

        JSValue val =
            JS_Eval(ctx, code.c_str(), code.size(), filename.c_str(), JS_EVAL_TYPE_GLOBAL);
        CheckJS(ctx, val);
    }

    static void Init()
    {
        std::string filename("scripts/Init.js");

        std::ifstream file(filename, std::ios::binary);
        std::string code =
            std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        JSValue global = JS_GetGlobalObject(ctx);

        // Register 'print' function
        JS_SetPropertyStr(ctx, global, "print",
                          JS_NewCFunction(ctx, print, "print", 1) // 1 arg
        );

        JS_FreeValue(ctx, global);


        JSValue val =
            JS_Eval(ctx, code.c_str(), code.size(), filename.c_str(), JS_EVAL_TYPE_GLOBAL);
        CheckJS(ctx, val);
    }

    static void DeInit()
    {
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
    }

    static JSRuntime* rt;
    static JSContext* ctx;
};
