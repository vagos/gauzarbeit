#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>
#include <algorithm>
#include <memory>
#include <vector>

class Thing;

bool IsNumber(const std::string& s);
bool PartlyMatch(const std::string& s1, const std::string& s2, int n = 3);

enum class Color
{
    None,
    White,
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
};

const std::string GetColor(Color color_code);
const std::string ColorString(const std::string &s, Color color_code);  

template<typename T>
const std::shared_ptr<Thing> GetSmartPtr(const T& container, Thing * t_ptr);

#endif//HELPERS_HPP
