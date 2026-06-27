#pragma once

#include "system/LLMSystem.hpp"
#include "thing/Thing.hpp"
#include <string>

class LLMThinker : public Thinker
{
  public:
    explicit LLMThinker(LLMSystem* llm_system, std::string prompt = "",
                        double cooldown_seconds = 15.0)
        : llm_system(llm_system), prompt(std::move(prompt)), cooldown_seconds(cooldown_seconds)
    {
    }

    void doThink(const std::shared_ptr<Thing>& owner, World& world) override;

  private:
    void executeCommand(const std::shared_ptr<Thing>& owner, World& world,
                        const std::string& command) const;
    std::string buildPrompt(const std::shared_ptr<Thing>& owner) const;

    LLMSystem* llm_system = nullptr;
    std::string prompt;
    double cooldown_seconds = 15.0;
    double next_think_time = 0.0;
    bool waiting_result = false;
};
