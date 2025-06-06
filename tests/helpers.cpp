#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Helpers.hpp"
#include <doctest/doctest.h>

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
	CHECK(GetColor(Color::Red) == "\u001b[31m");
	CHECK(GetColor(Color::Green) == "\u001b[32m");
}

TEST_CASE("BarString")
{
	CHECK(BarString(0.5f, 10) == "[:::::    ]");
}
