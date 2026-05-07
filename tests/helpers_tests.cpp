#include "Helpers.hpp"
#include "TestSupport.hpp"
#include <doctest/doctest.h>

TEST_CASE("CenteredString pads around content")
{
    CHECK_EQ(CenteredString("Hi", 6), "  Hi  ");
}

TEST_CASE("HeaderString includes title and body")
{
    auto header = HeaderString("body", "Title", '-', 8);
    CHECK(header.find("Title") != std::string::npos);
    CHECK(header.find("body") != std::string::npos);
    CHECK(header.back() == '\n');
}

TEST_CASE("BlockListString groups identical entries")
{
    std::vector<std::shared_ptr<Thing>> things{
        MakeBasicThing("Rat"),
        MakeBasicThing("Rat"),
        MakeBasicThing("Cheese"),
    };

    auto list = BlockListString(
        things, '*', [](const std::shared_ptr<Thing>& t) { return t->name; }, 3);

    CHECK(list.find("* Rat (2)") != std::string::npos);
    CHECK(list.find("* Cheese") != std::string::npos);
}

TEST_CASE("VerticalListString formats with separators")
{
    std::vector<std::shared_ptr<Thing>> things{
        MakeBasicThing("Apple"),
        MakeBasicThing("Banana"),
    };

    auto list = VerticalListString(
        things, '-', [](const auto& t) { return t->name; }, '#', 5);

    CHECK_EQ(list, "- Apple\n#####\n- Banana\n");
}

TEST_CASE("FindByName matches case-insensitively")
{
    auto apple = MakeBasicThing("Apple");
    auto banana = MakeBasicThing("banana");

    std::vector<std::shared_ptr<Thing>> things{apple, banana};

    CHECK(FindByName(things, "BANANA") == banana);
}
