#include "World.hpp"
#include "system/LLMSystem.hpp"
#include <chrono>
#include <doctest/doctest.h>
#include <optional>
#include <thread>

TEST_CASE("LLMSystem keeps only the latest pending prompt per entity")
{
    World world;

    LLMConfig config;
    config.model_path = "";
    config.max_pending_entities = 8;

    LLMSystem llm(world, config, [](const std::string& prompt)
                  {
                      std::this_thread::sleep_for(std::chrono::milliseconds(5));
                      return prompt;
                  });

    CHECK(llm.enqueueOrReplace(42, "first"));
    CHECK(llm.enqueueOrReplace(42, "second"));

    std::optional<std::string> result;
    for (int i = 0; i < 100; ++i)
    {
        llm.doUpdate(world);
        result = llm.pollResult(42);
        if (result)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    REQUIRE(result.has_value());
    CHECK(*result == "second");
}
