#pragma once

#include "Helpers.hpp"
#include <string>

class CommandParser
{
  public:
    static Event Parse(const std::string& request);
};
