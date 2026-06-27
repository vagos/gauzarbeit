#pragma once

#include "System.hpp"
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

struct LLMConfig
{
    std::string model_path = "Qwen3-4B-Instruct-2507-Q4_K_M.gguf";
    int n_predict = 24;
    int n_ctx = 2048;
    int n_gpu_layers = 0;
    std::size_t max_pending_entities = 256;
};

class LLMSystem : public System
{
  public:
    using InferFn = std::function<std::string(const std::string&)>;

    explicit LLMSystem(World& world, const LLMConfig& config = LLMConfig{}, InferFn infer_fn = {});
    ~LLMSystem();

    bool enqueueOrReplace(std::size_t entity_id, const std::string& prompt);
    std::optional<std::string> pollResult(std::size_t entity_id);

    void doUpdate(World& world) override;

  private:
    struct PendingJob
    {
        std::string prompt;
        std::uint64_t seq = 0;
    };

    void workerLoop();
    std::string infer(const std::string& prompt);

    mutable std::mutex mutex;
    std::condition_variable cv;

    LLMConfig config;
    InferFn infer_fn;

    std::unordered_map<std::size_t, PendingJob> pending_jobs;
    std::unordered_map<std::size_t, std::string> completed_results;
    std::unordered_map<std::size_t, std::string> ready_results;
    std::unordered_map<std::size_t, std::uint64_t> latest_seq;
    std::deque<std::size_t> pending_order;

    std::uint64_t next_seq = 1;
    std::atomic<bool> running{false};
    std::thread worker;
};
