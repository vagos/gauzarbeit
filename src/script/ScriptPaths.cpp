#include "script/ScriptPaths.hpp"
#include <cstdlib>
#include <filesystem>

// TODO: Move this to ScriptHelpers.hpp/cpp or similar

namespace
{
const char* kScriptRootEnv = "GAUZARBEIT_SCRIPT_ROOT";
}

std::string ScriptPaths::Resolve(const std::string& relative)
{
    const char* root = std::getenv(kScriptRootEnv);
    if (root && *root)
    {
        std::filesystem::path candidate = std::filesystem::path(root) / relative;
        if (std::filesystem::exists(candidate))
            return candidate.string();
    }

    return (std::filesystem::path("./scripts") / relative).string();
}

std::string ScriptPaths::ResolveDir(const std::string& dir)
{
    const char* root = std::getenv(kScriptRootEnv);
    if (root && *root)
    {
        std::filesystem::path candidate = std::filesystem::path(root) / dir;
        if (std::filesystem::exists(candidate))
            return candidate.string();
    }

    return (std::filesystem::path("./scripts") / dir).string();
}

std::string ScriptPaths::ResolveFromDir(const std::string& dir, const std::string& name,
                                        const std::string& extension)
{
    std::filesystem::path dir_path(dir);
    if (dir_path.is_absolute())
    {
        return (dir_path / (name + "." + extension)).string();
    }

    const char* root = std::getenv(kScriptRootEnv);
    if (root && *root)
    {
        std::string root_str(root);
        if (dir.rfind(root_str, 0) == 0 || dir.rfind("./" + root_str, 0) == 0)
        {
            return (dir_path / (name + "." + extension)).string();
        }
    }

    std::string normalized = dir;
    if (normalized.rfind("./scripts/", 0) == 0)
        normalized = normalized.substr(10);
    else if (normalized.rfind("scripts/", 0) == 0)
        normalized = normalized.substr(8);

    if (!normalized.empty())
    {
        std::filesystem::path rel = std::filesystem::path(normalized) / (name + "." + extension);
        return Resolve(rel.string());
    }

    return (std::filesystem::path(dir) / (name + "." + extension)).string();
}
