#ifndef HELPERS_HPP
#define HELPERS_HPP

constexpr int SIZE = 65;

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <filesystem>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define Log(msg) std::cerr << __FILE__ << ":" << __LINE__ << " " << msg << std::endl;

class Thing;
class World;
class Networked;
class Physical;
class Usable;
class Attackable;
class Talker;
class Notifier;
class Achiever;
class Tasker;
class Inspectable;
class Thinker;

bool IsNumber(const std::string& s);
bool PartlyMatch(const std::string& s1, const std::string& s2, int n = 3);
std::vector<std::string> TokenizeString(const std::string& s);
std::string CapitalizeWord(std::string word);
bool WithChance(double probability);

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
    Black,
};

enum class Style : unsigned int
{
    None = 0,
    Bold = 1u << 0,
    Faint = 1u << 1,
    Italic = 1u << 2,
    Underline = 1u << 3,
    Blink = 1u << 4,
    Reverse = 1u << 5,
    Conceal = 1u << 6,
    Strikethrough = 1u << 7,
    Bright = 1u << 8,
};

enum class Direction 
{
    North,
    South,
    East,
    West,
};

inline constexpr const char TerminalEscape[] = "\x1b";
inline constexpr const char TerminalCarriageReturn[] = "\r";
inline constexpr const char TerminalClearLine[] = "\x1b[2K";
inline constexpr const char TerminalReset[] = "\x1b[0m";
inline constexpr const char TerminalColorBlack[] = "\x1b[30m";
inline constexpr const char TerminalColorRed[] = "\x1b[31m";
inline constexpr const char TerminalColorGreen[] = "\x1b[32m";
inline constexpr const char TerminalColorYellow[] = "\x1b[33m";
inline constexpr const char TerminalColorBlue[] = "\x1b[34m";
inline constexpr const char TerminalColorMagenta[] = "\x1b[35m";
inline constexpr const char TerminalColorCyan[] = "\x1b[36m";
inline constexpr const char TerminalColorWhite[] = "\x1b[37m";
inline constexpr const char TerminalColorBrightBlack[] = "\x1b[90m";
inline constexpr const char TerminalColorBrightRed[] = "\x1b[91m";
inline constexpr const char TerminalColorBrightGreen[] = "\x1b[92m";
inline constexpr const char TerminalColorBrightYellow[] = "\x1b[93m";
inline constexpr const char TerminalColorBrightBlue[] = "\x1b[94m";
inline constexpr const char TerminalColorBrightMagenta[] = "\x1b[95m";
inline constexpr const char TerminalColorBrightCyan[] = "\x1b[96m";
inline constexpr const char TerminalColorBrightWhite[] = "\x1b[97m";
inline constexpr const char TerminalStyleBold[] = "\x1b[1m";
inline constexpr const char TerminalStyleFaint[] = "\x1b[2m";
inline constexpr const char TerminalStyleItalic[] = "\x1b[3m";
inline constexpr const char TerminalStyleUnderline[] = "\x1b[4m";
inline constexpr const char TerminalStyleBlink[] = "\x1b[5m";
inline constexpr const char TerminalStyleReverse[] = "\x1b[7m";
inline constexpr const char TerminalStyleConceal[] = "\x1b[8m";
inline constexpr const char TerminalStyleStrikethrough[] = "\x1b[9m";
inline constexpr const char PromptReset[] = "\r\x1b[2K";

struct Event
{
    enum class Type
    {
        Message,
        Say,
        Chat,
        Move,
        Enter,
        Use,
        Do,
        Buy,
        Info,
        Ask, // TODO: This one can be removed
        Help,
        Inspect,
        Attack,
        Attacked,
        Kill,
        Death,
        Leave,
        Gain,
        Provide,
        Register,
        Custom,
        Invalid,
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
const std::string ColorString(const std::string& s, Color color_code,
                              unsigned int style_mask = 0);
const std::string CenteredString(const std::string& s, int size = SIZE);
template <typename T> const std::shared_ptr<Thing> GetSmartPtr(const T& container, Thing* t_ptr);
const std::shared_ptr<Thing> FindByName(std::vector<std::shared_ptr<Thing>>& container,
                                        const std::string& s);

template <typename T, typename F>
const std::string BlockListString(const T& c, const char b, F f, int step = 3)
{
    std::unordered_map<std::string, int> counts;
    for (const auto& item : c)
        counts[f(item)] += 1;

    std::vector<std::string> cells;
    for (const auto& [name, count] : counts)
    {
        std::string cell;
        if (b)
            cell += std::string(1, b) + ' ';
        cell += name;
        if (count > 1)
            cell += " (" + std::to_string(count) + ')';
        cells.push_back(std::move(cell));
    }

    if (cells.empty())
        return "";

    const auto visible_length = [](const std::string& s)
    {
        std::size_t width = 0;
        bool in_escape = false;
        for (unsigned char ch : s)
        {
            if (in_escape)
            {
                if (ch == 'm')
                    in_escape = false;
                continue;
            }

            if (ch == '\x1b')
            {
                in_escape = true;
                continue;
            }

            ++width;
        }
        return width;
    };

    std::size_t column_width = 0;
    for (const auto& cell : cells)
        column_width = std::max(column_width, visible_length(cell));
    column_width += 2;

    std::stringstream ss;
    for (std::size_t i = 0; i < cells.size(); ++i)
    {
        ss << cells[i];

        const bool end_of_row = (i + 1) % std::max(1, step) == 0;
        const bool last = i + 1 == cells.size();
        if (end_of_row)
        {
            ss << '\n';
            continue;
        }

        if (!last)
        {
            const std::size_t visible_width = visible_length(cells[i]);
            if (visible_width < column_width)
                ss << std::string(column_width - visible_width, ' ');
            else
                ss << ' ';
        }
    }

    return ss.str();
}

const std::string BarString(float filled, int max_size = SIZE, const char f = ':',
                            const char lb = '[', const char rb = ']');
std::string TrimLine(std::string line);
std::vector<std::string> LoadLines(const std::filesystem::path& path);

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

#endif // HELPERS_HPP
