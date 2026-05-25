#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Helpers.hpp"
#include "script/lua/ScriptedThing.hpp"
#include <cstdlib>
#include <doctest/doctest.h>

void InitScriptVMsForTests()
{
    static bool initialized = false;
    if (initialized)
        return;

    setenv("GAUZARBEIT_SCRIPT_ROOT", "tests/ext", 1);
    ScriptedThing_Lua::Init();
    initialized = true;
}

// Specialization for std::vector<std::string> to work with doctest
namespace doctest
{
template <> struct StringMaker<std::vector<std::string>>
{
    static String convert(const std::vector<std::string>& vec)
    {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i)
                oss << ", ";
            oss << "'" << vec[i] << "'";
        }
        oss << "]";
        return oss.str().c_str();
    }
};
} // namespace doctest

TEST_CASE("IsNumber")
{
    CHECK(IsNumber("123"));
    CHECK(!IsNumber("12a3"));
    CHECK(!IsNumber(""));
}

TEST_CASE("PartlyMatch")
{
    CHECK(PartlyMatch("hello", "hel", 3));
    CHECK(!PartlyMatch("hello", "world", 3));
}

TEST_CASE("TokenizeString")
{
    auto tokens = TokenizeString("one two three  four");
    std::vector<std::string> expected{"one", "two", "three", "four"};
    CHECK_EQ(tokens, expected);
}

TEST_CASE("GetColor")
{
    CHECK(GetColor(Color::None) == "\u001b[0m");
    CHECK(GetColor(Color::Black) == "\u001b[30m");
    CHECK(GetColor(Color::Red) == "\u001b[31m");
    CHECK(GetColor(Color::Green) == "\u001b[32m");
}

TEST_CASE("ColorString styles")
{
    CHECK(ColorString("hello", Color::Red, static_cast<unsigned int>(Style::Bold)) ==
          "\u001b[31m\u001b[1mhello\u001b[0m");
    CHECK(ColorString("hello", Color::None, static_cast<unsigned int>(Style::Underline)) ==
          "\u001b[0m\u001b[4mhello\u001b[0m");
    CHECK(ColorString("hello", Color::Red, static_cast<unsigned int>(Style::Bright)) ==
          "\u001b[91mhello\u001b[0m");
}

TEST_CASE("Terminal control constants")
{
    CHECK(std::string(TerminalCarriageReturn) == "\r");
    CHECK(std::string(TerminalClearLine) == "\x1b[2K");
    CHECK(std::string(PromptReset) == "\r\x1b[2K");
}

TEST_CASE("SeedRNG makes WithChance deterministic")
{
    SeedRNG(12345);
    const bool first = WithChance(0.5);
    const bool second = WithChance(0.5);

    SeedRNG(12345);
    CHECK(WithChance(0.5) == first);
    CHECK(WithChance(0.5) == second);
    CHECK(!WithChance(0.0));
    CHECK(WithChance(1.0));
}

TEST_CASE("BarString")
{
    CHECK(BarString(0.5f, 10) == "[:::::    ]");
}
