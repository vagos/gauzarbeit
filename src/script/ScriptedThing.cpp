#include "script/ScriptedThing.hpp"
#include "script/ScriptPaths.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <filesystem>
#include <stdexcept>

std::shared_ptr<Thing> ScriptedThing(const std::string& name, const std::string& script_dir)
{
    const std::string lua_path = ScriptPaths::ResolveFromDir(script_dir, name, "lua");

    const bool has_lua = std::filesystem::exists(lua_path);

    if (has_lua)
    {
        return std::make_shared<ScriptedThing_Lua>(name, script_dir);
    }

    throw std::runtime_error("Scripted thing '" + name + "' not found in " + script_dir);
}
