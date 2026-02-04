#pragma once

#include "Helpers.hpp"
#include "Quest.hpp"
#include "Room.hpp"
#include "script/ScriptAPI.hpp"
#include "script/ScriptPaths.hpp"
#include "script/ScriptedThing.hpp"
#include "thing/Thing.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <memory>
#include <quickjs.h>
#include <sstream>
#include <string>
#include <vector>

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

inline std::shared_ptr<Thing> FindByPtr(const std::vector<std::shared_ptr<Thing>>& c, Thing* t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(),
                          [&t_ptr](const std::shared_ptr<Thing>& t) { return t.get() == t_ptr; });
    return r != c.end() ? *r : nullptr;
}

class ScriptedThing_JS : public Thing
{
  public:
    ScriptedThing_JS(const std::string& name, const std::string& script_dir = "./scripts/things/")
        : Thing(name)
    {
        _usable = std::make_unique<Usable>();
        _attackable = std::make_unique<Attackable>();
        _notifier = std::make_unique<Notifier>();
        _tasker = std::make_unique<Tasker>();
        _physical = std::make_unique<Physical>();
        _inspectable = std::make_unique<Inspectable>();
        _talker = std::make_unique<Talker>();
        _achiever = std::make_unique<Achiever>();
        _networked = std::make_unique<Networked>();

        std::string filename = ScriptPaths::ResolveFromDir(script_dir, name, "js");

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
        JS_FreeValue(ctx, val);

        JSValue fn = JS_GetPropertyStr(ctx, obj, "doInit");
        if (JS_IsFunction(ctx, fn))
        {
            JSValue ret = JS_Call(ctx, fn, obj, 1, &obj);
            JS_FreeValue(ctx, ret);
        }

        JS_FreeValue(ctx, fn);
        JS_FreeValue(ctx, global);
        JS_FreeValue(ctx, proto);
        JS_FreeValue(ctx, obj);
    }

    static Thing* self(JSContext* ctx, JSValueConst this_val)
    {
        return static_cast<Thing*>(JS_GetOpaque2(ctx, this_val, classID));
    }

    static Thing* argThing(JSContext* ctx, JSValueConst val)
    {
        if (!JS_IsObject(val))
            return nullptr;
        return static_cast<Thing*>(JS_GetOpaque2(ctx, val, classID));
    }

    static bool toString(JSContext* ctx, JSValueConst val, std::string& out)
    {
        const char* str = JS_ToCString(ctx, val);
        if (!str)
            return false;
        out.assign(str);
        JS_FreeCString(ctx, str);
        return true;
    }

    static JSValue newThingObject(JSContext* ctx, Thing* t, bool is_room = false)
    {
        JSValue global = JS_GetGlobalObject(ctx);
        JSValue proto = JS_GetPropertyStr(ctx, global, "__THING_PROTO__");
        JSValue obj = JS_NewObjectProtoClass(ctx, proto, classID);
        JS_SetOpaque(obj, t);
        if (is_room)
        {
            JS_SetPropertyStr(ctx, obj, "__isRoom", JS_NewBool(ctx, 1));
        }
        JS_FreeValue(ctx, proto);
        JS_FreeValue(ctx, global);
        return obj;
    }

    static JSValue getName(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t)
            return JS_EXCEPTION;
        std::string name = t->name;
        if (t->_inspectable)
        {
            try
            {
                name = t->inspectable()->getName(t->shared_from_this());
            }
            catch (const std::bad_weak_ptr&)
            {
                name = t->name;
            }
        }
        return JS_NewString(ctx, name.c_str());
    }

    static JSValue getType(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        return getName(ctx, this_val, argc, argv);
    }

    static JSValue sendMessage(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string message;
        if (!toString(ctx, argv[0], message))
            return JS_EXCEPTION;

        if (t->_networked)
            t->networked()->addResponse(message);

        return JS_UNDEFINED;
    }

    static JSValue loseItem(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        Thing* item_ptr = argThing(ctx, argv[0]);
        if (!item_ptr)
            return JS_UNDEFINED;

        auto item = FindByPtr(t->physical()->inventory, item_ptr);
        if (item)
            t->physical()->loseItem(item);
        return JS_UNDEFINED;
    }

    static JSValue dropItem(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        Thing* item_ptr = argThing(ctx, argv[0]);
        if (!item_ptr)
            return JS_UNDEFINED;

        auto item = FindByPtr(t->physical()->inventory, item_ptr);
        if (item)
            t->physical()->dropItem(item);
        return JS_UNDEFINED;
    }

    static JSValue equipItem(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        Thing* item_ptr = argThing(ctx, argv[0]);
        if (!item_ptr)
            return JS_UNDEFINED;

        auto item = FindByPtr(t->physical()->inventory, item_ptr);
        if (item)
            t->physical()->equipItem(item);
        return JS_UNDEFINED;
    }

    static JSValue getThing(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[0], name))
            return JS_EXCEPTION;

        auto thing = t->physical()->current_room->getThing(name);
        if (!thing)
            return JS_UNDEFINED;

        return newThingObject(ctx, thing.get());
    }

    static JSValue getPlayer(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[0], name))
            return JS_EXCEPTION;

        auto player = t->physical()->current_room->getPlayer(name);
        if (!player)
            return JS_UNDEFINED;

        return newThingObject(ctx, player.get());
    }

    static JSValue gainItem(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        if (JS_IsString(argv[0]))
        {
            std::string t_n;
            if (!toString(ctx, argv[0], t_n))
                return JS_EXCEPTION;
            auto item = std::make_shared<ScriptedThing>(t_n);
            t->physical()->gainItem(item);
            return JS_UNDEFINED;
        }

        Thing* item_ptr = argThing(ctx, argv[0]);
        if (!item_ptr)
            return JS_UNDEFINED;

        auto item = FindByPtr(t->physical()->current_room->things, item_ptr);
        if (!item)
            return JS_UNDEFINED;

        t->physical()->current_room->removeThing(item);
        t->physical()->gainItem(item);
        return JS_UNDEFINED;
    }

    static JSValue hasItem(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        Thing* item_ptr = argThing(ctx, argv[0]);
        if (!item_ptr)
            return JS_NewBool(ctx, 0);

        auto item = FindByPtr(t->physical()->inventory, item_ptr);
        return JS_NewBool(ctx, item && t->physical()->hasItem(item));
    }

    static JSValue broadcastMessage(JSContext* ctx, JSValueConst this_val, int argc,
                                    JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string message;
        if (!toString(ctx, argv[0], message))
            return JS_EXCEPTION;

        auto p = FindByPtr(t->physical()->getRoom()->players, t);
        if (!p)
            return JS_UNDEFINED;

        p->notifier()->setEventPayload(message);
        p->notifier()->doNotify(p, Event::Type::Message);
        return JS_UNDEFINED;
    }

    static JSValue addTask(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1 || !t->_tasker)
            return JS_UNDEFINED;

        std::string desc;
        if (!toString(ctx, argv[0], desc))
            return JS_EXCEPTION;

        int id = t->tasker()->addTask(desc);
        return JS_NewInt32(ctx, id);
    }

    static JSValue tickTask(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1 || !t->_tasker)
            return JS_UNDEFINED;

        int32_t idx = 0;
        if (JS_ToInt32(ctx, &idx, argv[0]))
            return JS_EXCEPTION;

        t->tasker()->tickTask(idx);
        return JS_UNDEFINED;
    }

    static JSValue gainXP(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1 || !t->_achiever)
            return JS_UNDEFINED;

        int32_t xp = 0;
        if (JS_ToInt32(ctx, &xp, argv[0]))
            return JS_EXCEPTION;

        t->achiever()->gainXP(xp);
        return JS_UNDEFINED;
    }

    static JSValue getLevel(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || !t->_achiever)
            return JS_UNDEFINED;

        return JS_NewInt32(ctx, t->achiever()->getLevel());
    }

    static JSValue getEventInfo(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || !t->_notifier)
            return JS_UNDEFINED;

        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "verb", JS_NewString(ctx, t->notifier()->event.verb.c_str()));
        JS_SetPropertyStr(ctx, obj, "target",
                          JS_NewString(ctx, t->notifier()->event.target.c_str()));
        JS_SetPropertyStr(ctx, obj, "object",
                          JS_NewString(ctx, t->notifier()->event.object.c_str()));
        JS_SetPropertyStr(ctx, obj, "extra", JS_NewString(ctx, t->notifier()->event.extra.c_str()));
        return obj;
    }

    static JSValue gainQuest(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string q_name;
        if (!toString(ctx, argv[0], q_name))
            return JS_EXCEPTION;

        t->achiever()->gainQuest(std::make_shared<ScriptedQuest>(q_name));
        return JS_UNDEFINED;
    }

    static JSValue doSay(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 2)
            return JS_EXCEPTION;

        Thing* target = argThing(ctx, argv[0]);
        if (!target)
            return JS_UNDEFINED;

        std::string message;
        if (!toString(ctx, argv[1], message))
            return JS_EXCEPTION;

        std::stringstream msg;
        msg << t->name << ": " << message;
        if (target->_networked)
            target->networked()->addResponse(msg.str());
        return JS_UNDEFINED;
    }

    static JSValue doAttack(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        Thing* target = argThing(ctx, argv[0]);
        if (!target)
            return JS_UNDEFINED;

        if (!(t->_attackable && target->_attackable))
            return JS_UNDEFINED;

        auto attacker = FindByPtr(t->physical()->current_room->things, t);
        auto victim = t->physical()->current_room->getAnything(target->name);
        if (!attacker || !victim)
            return JS_UNDEFINED;

        t->attackable()->doAttack(attacker, victim);
        return JS_UNDEFINED;
    }

    static JSValue getStat(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 1)
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[0], name))
            return JS_EXCEPTION;

        float val = t->achiever()->getStat(name);
        return JS_NewFloat64(ctx, val);
    }

    static JSValue setStat(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
    {
        auto t = self(ctx, this_val);
        if (!t || argc < 2)
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[0], name))
            return JS_EXCEPTION;

        double val = 0;
        if (JS_ToFloat64(ctx, &val, argv[1]))
            return JS_EXCEPTION;

        if (name == "MaxHealth")
        {
            t->attackable()->setMaxHealth((int)val);
        }
        else if (name == "Health")
        {
            t->attackable()->current_health = (float)val;
        }
        else if (name == "dmg")
        {
            t->attackable()->dmg = (int)val;
        }

        t->achiever()->setStat(name, (float)val);
        return JS_UNDEFINED;
    }

    static JSValue gauzarbeitSpawn(JSContext* ctx, JSValueConst this_val, int argc,
                                   JSValueConst* argv)
    {
        if (argc >= 2 && JS_IsObject(argv[0]))
        {
            JSValue flag = JS_GetPropertyStr(ctx, argv[0], "__isRoom");
            int is_room = JS_ToBool(ctx, flag);
            JS_FreeValue(ctx, flag);
            if (is_room)
            {
                Room* r = static_cast<Room*>(JS_GetOpaque2(ctx, argv[0], classID));
                if (!r)
                    return JS_EXCEPTION;

                std::string name;
                if (!toString(ctx, argv[1], name))
                    return JS_EXCEPTION;

                auto t = std::make_shared<ScriptedThing>(name);
                if (t->_physical)
                    t->physical()->doMove(t, r->x, r->y);
                else
                    r->addThing(t);

                return JS_UNDEFINED;
            }
        }

        if (argc < 3)
            return JS_EXCEPTION;

        int32_t x = 0;
        int32_t y = 0;
        if (JS_ToInt32(ctx, &x, argv[0]) || JS_ToInt32(ctx, &y, argv[1]))
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[2], name))
            return JS_EXCEPTION;

        auto r = Room::get(x, y);
        auto t = std::make_shared<ScriptedThing>(name);
        if (t->_physical)
            t->physical()->doMove(t, r->x, r->y);
        else
            r->addThing(t);

        return JS_UNDEFINED;
    }

    static JSValue gauzarbeitGetRoom(JSContext* ctx, JSValueConst this_val, int argc,
                                     JSValueConst* argv)
    {
        if (argc < 3)
            return JS_EXCEPTION;

        std::string name;
        if (!toString(ctx, argv[0], name))
            return JS_EXCEPTION;

        int32_t x = 0;
        int32_t y = 0;
        if (JS_ToInt32(ctx, &x, argv[1]) || JS_ToInt32(ctx, &y, argv[2]))
            return JS_EXCEPTION;

        auto r = Room::get(x, y);
        r->name = name;
        r->doGeneration();

        return newThingObject(ctx, r.get(), true);
    }

    static JSValue gauzarbeitColorString(JSContext* ctx, JSValueConst this_val, int argc,
                                         JSValueConst* argv)
    {
        if (argc < 2)
            return JS_EXCEPTION;

        std::string s;
        if (!toString(ctx, argv[0], s))
            return JS_EXCEPTION;

        int32_t color = 0;
        if (JS_ToInt32(ctx, &color, argv[1]))
            return JS_EXCEPTION;

        auto out = ColorString(s, (Color)color);
        return JS_NewString(ctx, out.c_str());
    }

    static JSValue gauzarbeitGetDBLine(JSContext* ctx, JSValueConst this_val, int argc,
                                       JSValueConst* argv)
    {
        std::string db_line;
        Networked::getDB() >> db_line;
        return JS_NewString(ctx, db_line.c_str());
    }

    static void Init()
    {
        std::string filename = ScriptPaths::Resolve("Init.js");
        std::ifstream file(filename, std::ios::binary);
        std::string code =
            std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        JSValue global = JS_GetGlobalObject(ctx);

        // Register 'print' function
        JS_SetPropertyStr(ctx, global, "print", JS_NewCFunction(ctx, print, "print", 1));

        JS_NewClassID(rt, &classID);
        JSClassDef def{};
        def.class_name = "Thing";
        def.finalizer = [](JSRuntime*, JSValue val)
        {
            // no-op; C++ owns Thing*
            (void)val;
        };
        JS_NewClass(rt, classID, &def);

        JSValue proto = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, proto, "getName",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getName, "getName", 0));
        JS_SetPropertyStr(ctx, proto, "getType",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getType, "getType", 0));
        JS_SetPropertyStr(ctx, proto, "setStat",
                          JS_NewCFunction(ctx, ScriptedThing_JS::setStat, "setStat", 2));
        JS_SetPropertyStr(ctx, proto, "getStat",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getStat, "getStat", 1));
        JS_SetPropertyStr(ctx, proto, "sendMessage",
                          JS_NewCFunction(ctx, ScriptedThing_JS::sendMessage, "sendMessage", 1));
        JS_SetPropertyStr(ctx, proto, "doSay",
                          JS_NewCFunction(ctx, ScriptedThing_JS::doSay, "doSay", 2));
        JS_SetPropertyStr(ctx, proto, "loseItem",
                          JS_NewCFunction(ctx, ScriptedThing_JS::loseItem, "loseItem", 1));
        JS_SetPropertyStr(ctx, proto, "dropItem",
                          JS_NewCFunction(ctx, ScriptedThing_JS::dropItem, "dropItem", 1));
        JS_SetPropertyStr(ctx, proto, "equipItem",
                          JS_NewCFunction(ctx, ScriptedThing_JS::equipItem, "equipItem", 1));
        JS_SetPropertyStr(ctx, proto, "getThing",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getThing, "getThing", 1));
        JS_SetPropertyStr(ctx, proto, "getPlayer",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getPlayer, "getPlayer", 1));
        JS_SetPropertyStr(ctx, proto, "gainItem",
                          JS_NewCFunction(ctx, ScriptedThing_JS::gainItem, "gainItem", 1));
        JS_SetPropertyStr(ctx, proto, "hasItem",
                          JS_NewCFunction(ctx, ScriptedThing_JS::hasItem, "hasItem", 1));
        JS_SetPropertyStr(
            ctx, proto, "broadcastMessage",
            JS_NewCFunction(ctx, ScriptedThing_JS::broadcastMessage, "broadcastMessage", 1));
        JS_SetPropertyStr(ctx, proto, "addTask",
                          JS_NewCFunction(ctx, ScriptedThing_JS::addTask, "addTask", 1));
        JS_SetPropertyStr(ctx, proto, "tickTask",
                          JS_NewCFunction(ctx, ScriptedThing_JS::tickTask, "tickTask", 1));
        JS_SetPropertyStr(ctx, proto, "gainXP",
                          JS_NewCFunction(ctx, ScriptedThing_JS::gainXP, "gainXP", 1));
        JS_SetPropertyStr(ctx, proto, "getEventInfo",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getEventInfo, "getEventInfo", 0));
        JS_SetPropertyStr(ctx, proto, "getLevel",
                          JS_NewCFunction(ctx, ScriptedThing_JS::getLevel, "getLevel", 0));
        JS_SetPropertyStr(ctx, proto, "gainQuest",
                          JS_NewCFunction(ctx, ScriptedThing_JS::gainQuest, "gainQuest", 1));
        JS_SetPropertyStr(ctx, proto, "doAttack",
                          JS_NewCFunction(ctx, ScriptedThing_JS::doAttack, "doAttack", 1));

        JS_DefinePropertyValueStr(ctx, global, "__THING_PROTO__", JS_DupValue(ctx, proto),
                                  JS_PROP_C_W_E);
        JS_SetClassProto(ctx, classID, proto);

        // Create Gauzarbeit global object
        JSValue gauzarbeit = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, gauzarbeit, "Spawn",
                          JS_NewCFunction(ctx, ScriptedThing_JS::gauzarbeitSpawn, "Spawn", 3));
        JS_SetPropertyStr(ctx, gauzarbeit, "GetRoom",
                          JS_NewCFunction(ctx, ScriptedThing_JS::gauzarbeitGetRoom, "GetRoom", 3));
        JS_SetPropertyStr(
            ctx, gauzarbeit, "ColorString",
            JS_NewCFunction(ctx, ScriptedThing_JS::gauzarbeitColorString, "ColorString", 2));
        JS_SetPropertyStr(
            ctx, gauzarbeit, "GetDBLine",
            JS_NewCFunction(ctx, ScriptedThing_JS::gauzarbeitGetDBLine, "GetDBLine", 0));

        JSValue event_obj = JS_NewObject(ctx);
        for (const ScriptConstant* c = ScriptAPI::kEventConstants; c->name; ++c)
        {
            JS_SetPropertyStr(ctx, event_obj, c->name, JS_NewInt32(ctx, c->value));
        }
        JS_SetPropertyStr(ctx, gauzarbeit, "Event", event_obj);

        JSValue color_obj = JS_NewObject(ctx);
        for (const ScriptConstant* c = ScriptAPI::kColorConstants; c->name; ++c)
        {
            JS_SetPropertyStr(ctx, color_obj, c->name, JS_NewInt32(ctx, c->value));
        }
        JS_SetPropertyStr(ctx, gauzarbeit, "Color", color_obj);

        JS_SetPropertyStr(ctx, global, "Gauzarbeit", gauzarbeit);

        JSValue val =
            JS_Eval(ctx, code.c_str(), code.size(), filename.c_str(), JS_EVAL_TYPE_GLOBAL);
        CheckJS(ctx, val);
        JS_FreeValue(ctx, val);

        VerifyJSAPI(ctx, proto);

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
