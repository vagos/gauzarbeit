#include "system/LLMSystem.hpp"
#include "Helpers.hpp"
#include "llama.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

namespace
{
void llama_log_callback_null(ggml_log_level, const char*, void*)
{
}

std::string InferWithLlama(const std::string& prompt, const LLMConfig& config)
{
    static std::once_flag llama_once;
    std::call_once(llama_once,
                   []
                   {
                       llama_log_set(llama_log_callback_null, nullptr);
                       ggml_backend_load_all();
                   });

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = config.n_gpu_layers;

    llama_model* model_raw = llama_model_load_from_file(config.model_path.c_str(), model_params);
    if (!model_raw)
        throw std::runtime_error("unable to load model: " + config.model_path);

    std::unique_ptr<llama_model, decltype(&llama_model_free)> model(model_raw, llama_model_free);

    const llama_vocab* vocab = llama_model_get_vocab(model.get());
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), static_cast<int>(prompt.size()),
                                         nullptr, 0, true, true);
    if (n_prompt <= 0)
        throw std::runtime_error("failed to tokenize prompt");

    std::vector<llama_token> prompt_tokens(n_prompt);
    if (llama_tokenize(vocab, prompt.c_str(), static_cast<int>(prompt.size()), prompt_tokens.data(),
                       static_cast<int>(prompt_tokens.size()), true, true) < 0)
    {
        throw std::runtime_error("failed to tokenize prompt data");
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = std::max(config.n_ctx, n_prompt + config.n_predict + 8);
    ctx_params.n_batch = n_prompt;
    ctx_params.no_perf = true;

    llama_context* ctx_raw = llama_init_from_model(model.get(), ctx_params);
    if (!ctx_raw)
        throw std::runtime_error("failed to create llama context");

    std::unique_ptr<llama_context, decltype(&llama_free)> ctx(ctx_raw, llama_free);

    auto sampler_params = llama_sampler_chain_default_params();
    sampler_params.no_perf = true;
    llama_sampler* smpl_raw = llama_sampler_chain_init(sampler_params);
    if (!smpl_raw)
        throw std::runtime_error("failed to create llama sampler");

    std::unique_ptr<llama_sampler, decltype(&llama_sampler_free)> smpl(smpl_raw, llama_sampler_free);
    llama_sampler_chain_add(smpl.get(), llama_sampler_init_top_k(40));
    llama_sampler_chain_add(smpl.get(), llama_sampler_init_top_p(0.9f, 1));
    llama_sampler_chain_add(smpl.get(), llama_sampler_init_temp(0.85f));
    llama_sampler_chain_add(smpl.get(), llama_sampler_init_dist(std::random_device{}()));

    llama_batch batch =
        llama_batch_get_one(prompt_tokens.data(), static_cast<int32_t>(prompt_tokens.size()));

    std::string output;
    output.reserve(256);

    for (int generated = 0; generated < config.n_predict; ++generated)
    {
        if (llama_decode(ctx.get(), batch) != 0)
            throw std::runtime_error("llama_decode failed");

        llama_token token = llama_sampler_sample(smpl.get(), ctx.get(), -1);
        if (llama_vocab_is_eog(vocab, token))
            break;

        char buf[128];
        const int n = llama_token_to_piece(vocab, token, buf, sizeof(buf), 0, true);
        if (n < 0)
            throw std::runtime_error("failed to convert token");

        output.append(buf, n);

        batch = llama_batch_get_one(&token, 1);
    }

    return output;
}
} // namespace

LLMSystem::LLMSystem(World& world, const LLMConfig& config, InferFn infer_fn)
    : System(world), config(config), infer_fn(std::move(infer_fn))
{
    if (!this->infer_fn && !std::filesystem::exists(this->config.model_path))
        throw std::runtime_error("model file not found at " + this->config.model_path);

    running = true;
    worker = std::thread(&LLMSystem::workerLoop, this);
}

LLMSystem::~LLMSystem()
{
    running = false;
    cv.notify_all();

    if (worker.joinable())
        worker.join();

    std::lock_guard<std::mutex> lock(mutex);
    pending_jobs.clear();
    completed_results.clear();
    ready_results.clear();
    latest_seq.clear();
    pending_order.clear();
}

bool LLMSystem::enqueueOrReplace(std::size_t entity_id, const std::string& prompt)
{
    if (!running)
        return false;

    std::lock_guard<std::mutex> lock(mutex);

    auto& seq = latest_seq[entity_id];
    seq = next_seq++;

    auto it = pending_jobs.find(entity_id);
    if (it != pending_jobs.end())
    {
        it->second.prompt = prompt;
        it->second.seq = seq;
        cv.notify_one();
        return true;
    }

    if (pending_jobs.size() >= config.max_pending_entities)
        return false;

    pending_jobs[entity_id] = PendingJob{prompt, seq};
    pending_order.push_back(entity_id);
    cv.notify_one();
    return true;
}

std::optional<std::string> LLMSystem::pollResult(std::size_t entity_id)
{
    std::lock_guard<std::mutex> lock(mutex);

    auto it = ready_results.find(entity_id);
    if (it == ready_results.end())
        return std::nullopt;

    std::string out = it->second;
    ready_results.erase(it);
    return out;
}

void LLMSystem::doUpdate(World&)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [entity_id, result] : completed_results)
    {
        ready_results[entity_id] = std::move(result);
    }
    completed_results.clear();
}

void LLMSystem::workerLoop()
{
    while (true)
    {
        std::size_t entity_id = 0;
        PendingJob job;

        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return !running || !pending_order.empty(); });

            if (!running && pending_order.empty())
                return;

            if (pending_order.empty())
                continue;

            entity_id = pending_order.front();
            pending_order.pop_front();

            auto it = pending_jobs.find(entity_id);
            if (it == pending_jobs.end())
                continue;

            job = it->second;
            pending_jobs.erase(it);
        }

        std::string result;
        try
        {
            result = infer(job.prompt);
        }
        catch (const std::exception& e)
        {
            Log("LLM inference failed: " << e.what());
            result.clear();
        }

        std::lock_guard<std::mutex> lock(mutex);
        auto latest = latest_seq.find(entity_id);
        if (latest == latest_seq.end() || latest->second != job.seq)
            continue;

        if (!result.empty())
            completed_results[entity_id] = result;
    }
}

std::string LLMSystem::infer(const std::string& prompt)
{
    InferFn infer_local;
    LLMConfig config_local;

    {
        std::lock_guard<std::mutex> lock(mutex);
        infer_local = infer_fn;
        config_local = config;
    }

    if (infer_local)
        return infer_local(prompt);

    return InferWithLlama(prompt, config_local);
}
