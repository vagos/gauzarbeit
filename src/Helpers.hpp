#ifndef HELPERS_HPP
#define HELPERS_HPP

constexpr int SIZE = 40;

#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <list>
#include <sstream>

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

const std::string HeaderString(const std::string &s, const std::string &title, const char h = '=', int size = SIZE);

const std::string ColorString(const std::string &s, Color color_code);  

template<typename T>
const std::shared_ptr<Thing> GetSmartPtr(const T& container, Thing * t_ptr);

std::vector<std::string> TokenizeString(const std::string& s);

template<typename T, typename F>
const std::string VerticalListString(const T& c, const char b, F f, const char sep = '-', int size = SIZE)
{
    std::stringstream ss;

    for (auto it = c.begin(); it != c.end(); ++it)
    {
        ss << b << ' ' << f(**it) << '\n'; 

        if (size && it != c.end() - 1) ss << std::string(size, sep) << '\n';
    }

    return ss.str();
}


template<typename T>
const std::string VerticalListString(const T& c, const char b, const char sep = '-', int size = SIZE)
{
    std::stringstream ss;

    for (auto it = c.begin(); it != c.end(); ++it)
    {
        ss << b << ' ' << **it << '\n'; 

        if (size && it != c.end() - 1) ss << std::string(size, sep) << '\n';
    }

    return ss.str();
}


#endif//HELPERS_HPP
