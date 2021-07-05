#ifndef HELPERS_HPP
#define HELPERS_HPP

constexpr int SIZE = 65;

#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include <exception>

class Thing;

bool IsNumber(const std::string& s);
bool PartlyMatch(const std::string& s1, const std::string& s2, int n = 3);
std::vector<std::string> TokenizeString(const std::string& s);


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

const std::string HeaderString(const std::string &s, const std::string &title, const char h = ' ', int size = SIZE);

const std::string ColorString(const std::string &s, Color color_code);  

const std::string CenteredString(const std::string &s, int size = SIZE);

template<typename T>
const std::shared_ptr<Thing> GetSmartPtr(const T& container, Thing * t_ptr);


const std::shared_ptr<Thing> FindByName(std::vector<std::shared_ptr<Thing>> &container, const std::string &s);


template<typename T>
const std::string BlockListString(const T& c, const char b, int step = 3, int size = SIZE)
{
    int i = 0;

    std::stringstream ss;

    for (auto t : c)
    {
        ss << b << ' ' << *t << '\t';
        
        i++;

        if (i % step == 0) ss << '\n'; 

    }

    return ss.str();
}

const std::string BarString(float filled, int max_size = SIZE, const char f = ':', const char lb = '[', const char rb =']');

template<typename T, typename F>
const std::string VerticalListString(const T& c, const char b, F f, const char sep = ' ', int size = SIZE)
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
const std::string VerticalListString(const T& c, const char b, const char sep = ' ', int size = SIZE)
{
    std::stringstream ss;

    for (auto it = c.begin(); it != c.end(); ++it)
    {
        ss << b << ' ' << **it << '\n'; 

        if (size && it != c.end() - 1) ss << std::string(size, sep) << '\n';
    }

    return ss.str();
}

void HandleException(const std::shared_ptr<Thing>& t, std::exception& e );

#endif//HELPERS_HPP
