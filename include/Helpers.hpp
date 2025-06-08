#ifndef HELPERS_HPP
#define HELPERS_HPP

constexpr int SIZE = 65;

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define Log(msg) std::cerr << __FILE__ << ":" << __LINE__ << " " << msg << std::endl;

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

struct Event
{
    enum class Type
    {
        Invalid,
        Say,
        Move,
        Use,
        Do,
        Whisper,
        Chat,
        Message,
        Buy,
        Info,
        Ask,
        Help,
        Inspect,
        Greet,
        Attack,
        Kill,
        Death,
        Enter,
        Leave,
        Gain,
        Look,
        Gain_Quest,
        Register,
    };

    std::string verb;
    std::string target;
    std::string object;
    std::string extra;
    std::string payload;

    Type type;
};

const std::string GetColor(Color color_code);

const std::string HeaderString(const std::string& s, const std::string& title, const char h = ' ',
                               int size = SIZE);

const std::string ColorString(const std::string& s, Color color_code);

const std::string CenteredString(const std::string& s, int size = SIZE);

template <typename T> const std::shared_ptr<Thing> GetSmartPtr(const T& container, Thing* t_ptr);

const std::shared_ptr<Thing> FindByName(std::vector<std::shared_ptr<Thing>>& container,
                                        const std::string& s);

template <typename T, typename F>
const std::string BlockListStringSimple(const T& c, const char b, F f, int step = 3,
                                        int size = SIZE)
{
    int i = 0;

    std::stringstream ss;

    for (auto& t : c)
    {
        ss << b << (b ? ' ' : '\0') << f(t);

        ss << '\t';

        i++;

        if (i % step == 0)
            ss << '\n';
    }

    return ss.str();
}

template <typename T, typename F>
const std::string BlockListString(const T& c, const char b, F f, int step = 3, int size = SIZE)
{
    // Group identical Things

    std::unordered_map<std::string, int> m;

    for (auto& t : c)
    {
        m[f(t)] += 1;
    }

    std::stringstream ss;

    int i = 0;

    for (auto [name, n] : m)
    {

        ss << b << ' ' << name;

        if (n > 1)
            ss << " (" << n << ')';

        ss << '\t';

        i++;

        if (i % step == 0)
            ss << '\n';
    }

    return ss.str();
}

const std::string BarString(float filled, int max_size = SIZE, const char f = ':',
                            const char lb = '[', const char rb = ']');

template <typename T, typename F>
const std::string VerticalListString(const T& c, const char b, F f, const char sep = 0,
                                     int size = SIZE)
{
    std::stringstream ss;

    for (auto it = c.begin(); it != c.end(); ++it)
    {
        ss << b << ' ' << f(*it) << '\n';

        if (size && sep && it != c.end() - 1)
            ss << std::string(size, sep) << '\n';
    }

    return ss.str();
}

template <typename T>
const std::string VerticalListString(const T& c, const char b, const char sep = ' ',
                                     int size = SIZE)
{
    std::stringstream ss;

    for (auto it = c.begin(); it != c.end(); ++it)
    {
        ss << b << ' ' << **it << '\n';

        if (size && it != c.end() - 1)
            ss << std::string(size, sep) << '\n';
    }

    return ss.str();
}

void HandleException(const std::shared_ptr<Thing>& t, std::exception& e);

void DoFight(const std::shared_ptr<Thing>& t1, const std::shared_ptr<Thing>& t2);

#endif // HELPERS_HPP
