#include "Helpers.hpp"
#include "Exceptions.hpp"
#include "thing/Thing.hpp"
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <iostream>
#include <memory>
#include <random>
#include <regex>
#include <sstream>

bool IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

bool PartlyMatch(const std::string& s1, const std::string& s2, int n)
{
    auto it1 = s1.begin(), it2 = s2.begin();

    int m = 0;

    while (it1 != s1.end() && it2 != s2.end() && m < n)
    {
        if (*it1 == *it2)
            m++;
        else
            break;

        ++it1;
        ++it2;
    }

    return m == n;
}

std::vector<std::string> TokenizeString(const std::string& s)
{
    auto const re = std::regex{R"(\s+)"};

    return std::vector<std::string>(std::sregex_token_iterator{begin(s), end(s), re, -1},
                                    std::sregex_token_iterator{});
}

std::string CapitalizeWord(std::string word)
{
    if (!word.empty())
        word[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(word[0])));

    return word;
}

bool WithChance(double probability)
{
    if (probability <= 0.0)
        return false;
    if (probability >= 1.0)
        return true;

    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(rng) < probability;
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

const std::string ColorString(const std::string& s, Color color_code)
{
    return GetColor(color_code) + s + GetColor(Color::None);
}

const std::string CenteredString(const std::string& s, int size)
{
    int g = (size - s.size()) / 2;

    std::string c(g, ' ');

    c += s;

    c.resize(size, ' ');

    return c;
}

const std::string HeaderString(const std::string& s, const std::string& title, const char h,
                               int size)
{
    std::string r(CenteredString(title, size));

    r.resize(size, h);
    r += '\n';
    r += s;
    r.resize(r.size() + size, h);
    r += '\n';

    return r;
}

const std::string BarString(float filled, int max_size, const char f, const char lb, const char rb)
{
    std::string s(1, lb);

    s.resize(int(max_size * filled + 1), ':');

    s.resize(max_size, ' ');

    s += rb;

    return s;
}

std::string TrimLine(std::string line)
{
    if (line.rfind("\xEF\xBB\xBF", 0) == 0)
        line.erase(0, 3);

    const std::string whitespace = " \t\r\n";
    const std::size_t first = line.find_first_not_of(whitespace);
    if (first == std::string::npos)
        return "";

    const std::size_t last = line.find_last_not_of(whitespace);
    return line.substr(first, last - first + 1);
}

std::vector<std::string> LoadLines(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        Log("World generation could not open " << path.string());
        return {};
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        line = TrimLine(line);
        if (!line.empty())
            lines.push_back(line);
    }

    return lines;
}

class Thing;
template <typename T> const std::shared_ptr<Thing> GetSmartPtr(const T& c, Thing* t_ptr)
{
    auto r = std::find_if(c.begin(), c.end(),
                          [&t_ptr](const std::shared_ptr<Thing>& t) { return t.get() == t_ptr; });

    return r != c.end() ? *r : nullptr;
}

const std::shared_ptr<Thing> FindByName(std::vector<std::shared_ptr<Thing>>& c,
                                        const std::string& s)
{
    auto r = std::find_if(c.begin(), c.end(),
                          [&s](const auto& i) { return boost::iequals(s, i->name); });

    return r != c.end() ? *r : nullptr;
}

void HandleException(const std::shared_ptr<Thing>& t, std::exception& e)
{
    if (!t->_networked)
        return;

    t->networked()->addResponse(ColorString(e.what(), Color::Red));

    return;
}
