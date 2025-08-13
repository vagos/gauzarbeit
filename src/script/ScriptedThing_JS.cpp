#include "script/ScriptedThing_JS.hpp"

JSRuntime* ScriptedThing_JS::rt = JS_NewRuntime();
JSContext* ScriptedThing_JS::ctx = JS_NewContext(ScriptedThing_JS::rt);
