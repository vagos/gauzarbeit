#pragma once

#include "system/LLMSystem.hpp"
#include "thing/Thing.hpp"
#include <deque>
#include <optional>
#include <string>

class LLMNotifier : public Notifier
{
  public:
    explicit LLMNotifier(int cooldown_ticks = 10, int memory_turns = 8,
                         LLMSystem* llm_system = nullptr);

    struct Stimulus
    {
        Event::Type type = Event::Type::Invalid;
        std::string actor_name;
        std::string payload;
        std::string target_name;
    };

    void onNotify(const std::shared_ptr<Thing>& owner, const std::shared_ptr<Thing>& actor,
                  Event::Type notification_type,
                  const std::shared_ptr<Thing>& target = nullptr) override;
    void doUpdate(const std::shared_ptr<Thing>& owner) override;

  private:
    std::string buildPrompt(const std::shared_ptr<Thing>& owner,
                            const Stimulus& stimulus) const;
    std::string sanitizeReply(const std::string& raw) const;
    void remember(const std::string& line);
    void broadcast(const std::shared_ptr<Thing>& owner, const std::string& reply) const;
    std::string buildAskPayload(const std::shared_ptr<Thing>& actor) const;
    std::string buildChatPayload(const std::shared_ptr<Thing>& actor) const;

    std::optional<Stimulus> latest_stimulus;
    std::deque<std::string> memory;
    int cooldown_ticks = 10;
    int memory_turns = 8;
    int ticks_until_next = 0;
    bool waiting_result = false;
    LLMSystem* llm_system = nullptr;
};
