#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "Helpers.hpp"

TEST_CASE("IsNumber") {
    CHECK(IsNumber("123"));
    CHECK(!IsNumber("12a3"));
    CHECK(!IsNumber(""));
}

TEST_CASE("PartlyMatch") {
    CHECK(PartlyMatch("hello", "HELLO", 3));
    CHECK(!PartlyMatch("hello", "world", 3));
}

TEST_CASE("TokenizeString") {
    auto tokens = TokenizeString("one two,three!");
    std::vector<std::string> expected{"one", "two", "three"};
    CHECK(tokens == expected);
}

TEST_CASE("GetColor") {
    CHECK(GetColor(Color::None)  == "\u001b[0m");
    CHECK(GetColor(Color::Red)   == "\u001b[31m");
    CHECK(GetColor(Color::Green) == "\u001b[32m");
}

TEST_CASE("BarString") {
    CHECK(BarString(0.5f, 10) == "[:::::    ]");
}
