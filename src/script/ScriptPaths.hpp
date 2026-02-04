#pragma once

#include <string>

namespace ScriptPaths
{
std::string Resolve(const std::string& relative);
std::string ResolveDir(const std::string& dir);
std::string ResolveFromDir(const std::string& dir, const std::string& name,
                           const std::string& extension);
} // namespace ScriptPaths
