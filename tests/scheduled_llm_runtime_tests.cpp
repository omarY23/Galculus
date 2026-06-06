#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "galculus/llm/LLMRuntime.hpp"
#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/LLMResult.hpp"

#include "galculus/scheduler/TokenBudget.hpp"
#include "galculus/scheduler/TokenScheduler.hpp"

#include "galculus/llm_scheduled/ScheduledLLMRuntime.hpp"

using namespace galculus::llm;
using namespace galculus::scheduler;
using namespace galculus::llm_scheduled;

class InspectingLLM final : public LLMRuntime {
public:
    LLMResult generate(
        const Prompt&,
        const GenerationConfig& config
    ) override {
        called = true;
        received_max_tokens = config.max_tokens;

        LLMResult result;
        result.text = "max_tokens=" + std::to_string(config.max_tokens);
        result.prompt_tokens = 10;
        result.completion_tokens = config.max_tokens;
        result.elapsed_ms = 1;
        result.error_message = "";
        return result;
    }

    std::string name() const override {
        return "InspectingLLM";
    }

    bool called = false;
    std::uint32_t received_max_tokens = 0;
};

static TokenBudget make_budget(
    std::uint32_t max_prompt,
    std::uint32_t max_completion,
    std::uint32_t remaining
) {
    TokenBudget budget;
    budget.max_prompt_tokens = max_prompt;
    budget.max_completion_tokens = max_completion;
    budget.remaining_tokens = remaining;
    return budget;
}

static Prompt make_prompt() {
    Prompt prompt;
    prompt.add_system("You are a safe embedded planner.");
    prompt.add_user("Motion detected near the door.");
    return prompt;
}

static GenerationConfig make_config(std::uint32_t max_tokens) {
    GenerationConfig config;
    config.max_tokens = max_tokens;
    config.temperature = 0.1f;
    config.top_p = 0.9f;
    config.timeout_ms = 1000;
    config.stream = false;
    return config;
}

static void test_allow_calls_inner_llm() {
    InspectingLLM inner;
    TokenScheduler scheduler(make_budget(128, 64, 192));
    ScheduledLLMRuntime scheduled(inner, scheduler);

    auto result = scheduled.generate(make_prompt(), make_config(32));

    assert(inner.called);
    assert(inner.received_max_tokens == 32);
    assert(result.error_message.empty());
}

static void test_reduce_tokens_changes_config() {
    InspectingLLM inner;
    TokenScheduler scheduler(make_budget(128, 16, 144));
    ScheduledLLMRuntime scheduled(inner, scheduler);

    auto result = scheduled.generate(make_prompt(), make_config(64));

    assert(inner.called);
    assert(inner.received_max_tokens == 16);
    assert(result.error_message.empty());
    assert(result.text == "max_tokens=16");
}

static void test_fallback_when_prompt_too_large() {
    InspectingLLM inner;
    TokenScheduler scheduler(make_budget(4, 16, 20));
    ScheduledLLMRuntime scheduled(inner, scheduler);

    Prompt prompt;
    prompt.add_user(
        "This is a long prompt that should exceed the tiny prompt budget."
    );

    auto result = scheduled.generate(prompt, make_config(8));

    assert(!inner.called);
    assert(result.error_message.empty());
    assert(result.text.find("tool=send_alert") != std::string::npos);
}

int main() {
    test_allow_calls_inner_llm();
    test_reduce_tokens_changes_config();
    test_fallback_when_prompt_too_large();

    std::cout << "[scheduled_llm_runtime_tests] all tests passed\n";
    return 0;
}