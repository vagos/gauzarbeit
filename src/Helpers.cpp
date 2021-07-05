#include <memory>
#include <iostream>
#include <sstream>
#include <regex>

#include "Exceptions.hpp"
#include "Helpers.hpp"
#include "Thing.hpp"

bool IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();s.end();
}

bool PartlyMatch(const std::string& s1, const std::string& s2, int n)
{
    auto it1 = s1.begin(), it2 = s2.begin();

    int m = 0;

    while (it1 != s1.end() && it2 != s2.end() && m < n)
    {
        if (*it1 == *it2) m++;
        else break;
        
        ++it1;
        ++it2;
    }

    return m == n;
}

std::vector<std::string> TokenizeString(const std::string &s)
{
    auto const re = std::regex{R"(\s+)"};
    
    return std::vector<std::string>(
        std::sregex_token_iterator{begin(s), end(s), re, -1},
        std::sregex_token_iterator{});
}

const std::string GetColor(Color color_code)
{
    switch (color_code)
    {
        case Color::Red:
            return "\u001b[31m"; 
        case Color::Blue:
            return "\u001b[34m"; 
        case Color::Green:
            return "\u001b[32m"; 
        case Color::None: 
            return "\u001b[0m";
        case Color::Yellow:
            return "\u001b[33m"; 
        default:
            return "";
    }
}

const std::string ColorString(const std::string &s, Color color_code)
{
    return GetColor(color_code) + s + GetColor(Color::None);
}

const std::string CenteredString(const std::string &s, int size)
{
    int g = (size - s.size()) / 2;

    std::string c(g, ' ');

    c += s;

    c.resize(size, ' ');

    return c;
}

const std::string HeaderString(const std::string &s, const std::string &title, const char h, int size)
{
    std::string r( CenteredString(title, size) );

    r.resize( size, h ); 
    r += '\n';
    r += s;
    r.resize(r.size() + size, h);
    r += '\n';

    return r;
}

const std::string BarString(float filled, int max_size, const char f, const char lb, const char rb)
{
    std::string s(1, lb);

    std::clog << filled << '\n';

    s.resize(int(max_size * filled + 1), ':');

    s.resize(max_size, ' ');

    s += rb;

    return s;
}


class Thing;
template<typename T>
const std::shared_ptr<Thing> GetSmartPtr(const T& c, Thing * t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(), [&t_ptr](const std::shared_ptr<Thing> &t)
            {return t.get() == t_ptr;});    
    
    return r != c.end() ? *r : nullptr;
}

const std::shared_ptr<Thing> FindByName(std::vector<std::shared_ptr<Thing>> &c, const std::string &s)
{
   auto r = std::find_if(c.begin(), c.end(), 
           [&s](const auto& i){return i -> name == s;});

   return r != c.end() ? *r : nullptr;
}



void HandleException(const std::shared_ptr<Thing> &t, std::exception &e)
{
    if (!t -> _networked) return;

    t -> networked() -> addResponse( ColorString( e.what(), Color::Red ) );

    return;
}
