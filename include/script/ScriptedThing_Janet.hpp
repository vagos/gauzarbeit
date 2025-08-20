#pragma once

#include "thing/Thing.hpp"
#include <janet.h>
#include <fstream>
#include <iostream>

#define CheckJanet(ret) \
    do { \
        if (janet_checktype(ret, JANET_NIL) && janet_unwrap_nil(ret) == janet_wrap_nil()) { \
            /* Nil is OK */ \
        } else if (janet_checktype(ret, JANET_STRING)) { \
            /* String results are fine */ \
        } else if (janet_checktype(ret, JANET_NUMBER)) { \
            /* Number results are fine */ \
        } else if (janet_checktype(ret, JANET_BOOLEAN)) { \
            /* Boolean results are fine */ \
        } else { \
            /* Other types are OK too, this is just a placeholder for error checking */ \
        } \
    } while (0)

// Native C++ function we want to expose to Janet
static Janet print_janet(int32_t argc, Janet *argv)
{
    if (argc < 1 || !janet_checktype(argv[0], JANET_STRING)) {
        janet_panic("print: Expected a string argument.");
    }
    
    const char* str = (const char*)janet_unwrap_string(argv[0]);
    std::clog << str << std::endl;
    return janet_wrap_nil();
}

class ScriptedThing_Janet : public Thing
{
  public:
    ScriptedThing_Janet(const std::string& name, const std::string& script_dir = "./scripts/things/")
        : Thing(name)
    {
        Log("Creating Janet scripted thing: " << name << std::endl);
        
        std::string filename(script_dir + name + ".janet");
        
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            Log("Janet: Could not open script file: " << filename << ", continuing without script" << std::endl);
            return;
        }
        
        std::string code = std::string((std::istreambuf_iterator<char>(file)), 
                                     std::istreambuf_iterator<char>());
        
        // Compile and run the script code
        JanetCompileResult result = janet_compile(janet_cstringv(code.c_str()), env, janet_cstring(filename.c_str()));
        if (result.status != JANET_COMPILE_OK) {
            Log("Janet: Compilation error in " << filename << std::endl);
            return;
        }
        
        // Create a function from the funcdef and run it
        JanetFunction* compiled_fn = janet_thunk(result.funcdef);
        Janet run_result;
        JanetSignal signal = janet_pcall(compiled_fn, 0, NULL, &run_result, NULL);
        if (signal != JANET_SIGNAL_OK) {
            Log("Janet: Runtime error in " << filename << ": signal=" << signal << std::endl);
            return;
        }
        
        Log("Janet: Script " << filename << " loaded successfully" << std::endl);
        
        // TODO: Call doInit if it exists once function lookup is implemented
    }
    
    ~ScriptedThing_Janet() {
        // Janet handles cleanup
    }

    static Thing* self_from_janet(Janet *argv) {
        if (!janet_checktype(argv[0], JANET_ABSTRACT)) {
            janet_panic("Expected Thing instance");
        }
        return static_cast<Thing*>(janet_unwrap_abstract(argv[0]));
    }

    static Janet getName_janet(int32_t argc, Janet *argv) {
        if (argc < 1) janet_panic("getName: Expected thing argument");
        Thing* t = self_from_janet(argv);
        return janet_cstringv(t->name.c_str());
    }

    static void Init() {
        // Initialize Janet
        janet_init();
        
        // Create environment
        env = janet_core_env(NULL);
        
        Log("Janet scripting system initialized" << std::endl);
        
        // Load initialization script
        std::string filename("scripts/Init.janet");
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            std::string code = std::string((std::istreambuf_iterator<char>(file)), 
                                         std::istreambuf_iterator<char>());
            
            JanetCompileResult result = janet_compile(janet_cstringv(code.c_str()), env, janet_cstring(filename.c_str()));
            if (result.status == JANET_COMPILE_OK) {
                JanetFunction* compiled_fn = janet_thunk(result.funcdef);
                Janet run_result;
                JanetSignal signal = janet_pcall(compiled_fn, 0, NULL, &run_result, NULL);
                if (signal != JANET_SIGNAL_OK) {
                    Log("Janet: Error in Init.janet: signal=" << signal << std::endl);
                } else {
                    Log("Janet: Init.janet loaded successfully" << std::endl);
                }
            } else {
                Log("Janet: Compilation error in Init.janet" << std::endl);
            }
        } else {
            Log("Janet: Init.janet not found, continuing without it" << std::endl);
        }
    }

    static void DeInit() {
        if (env) {
            // Janet handles cleanup automatically
            env = NULL;
        }
        janet_deinit();
    }

    static JanetTable* env;
};