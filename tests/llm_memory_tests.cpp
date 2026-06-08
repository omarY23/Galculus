#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "galculus/llm/LLMRuntime.hpp"
#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/LLMResult.hpp"

#include "galculus/memory/MemoryManager.hpp"
#include "galculus/llm_memory/MemoryAwareLLMRuntime.hpp"

using namespace galculus::llm;
using namespace galculus::memory;
using namespace galculus::llm_memory;

class TestLLM final : public LLMRuntime {
public:
    LLMResult generate(
        const Prompt&,
        const GenerationConfig&
    ) override {
        called = true;

        LLMResult result;
        result.text = "ok";
        result.error_message = "";
        result.prompt_tokens = 4;
        result.completion_tokens = 8;
        result.elapsed_ms = 1;
        return result;
    }

    std::string name() const override {
        return "TestLLM";
    }

    bool called = false;
};

static Prompt make_prompt() {
    Prompt prompt;
    prompt.add_system("You are a safe embedded planner.");
    prompt.add_user("Motion detected near the door.");
    return prompt;
}

static GenerationConfig make_config() {
    GenerationConfig config;
    config.max_tokens = 32;
    config.temperature = 0.1f;
    config.top_p = 0.9f;
    config.timeout_ms = 1000;
    config.stream = false;
    return config;
}

static bool test_memory_aware_llm_allows_when_budget_available() {
    MemoryManagerConfig mem_config;
    mem_config.global_arena_size = 4096;
    mem_config.scratch_arena_size = 2048;

    MemoryManager memory(mem_config);
    memory.set_agent_budget("planner", 4096);

    TestLLM inner;

    MemoryAwareLLMRuntime llm(
        inner,
        memory,
        "planner"
    );

    auto result =
        llm.generate(
            make_prompt(),
            make_config()
        );

    if (!inner.called) {
        return false;
    }

    if (!result.error_message.empty()) {
        return false;
    }

    const auto* budget =
        memory.agent_budget("planner");

    if (budget == nullptr) {
        return false;
    }

    if (budget->used_bytes() != 0) {
        return false;
    }

    return true;
}

static bool test_memory_aware_llm_rejects_when_budget_exceeded() {
    MemoryManagerConfig mem_config;
    MemoryManager memory(mem_config);

    memory.set_agent_budget("planner", 16);

    TestLLM inner;

    MemoryAwareLLMRuntime llm(
        inner,
        memory,
        "planner"
    );

    auto result =
        llm.generate(
            make_prompt(),
            make_config()
        );

    if (inner.called) {
        return false;
    }

    if (result.error_message.empty()) {
        return false;
    }

    return true;
}

int main() {
    int failed = 0;

    failed += !test_memory_aware_llm_allows_when_budget_available();
    failed += !test_memory_aware_llm_rejects_when_budget_exceeded();

    if (failed != 0) {
        std::cerr << "[llm_memory_tests] failed tests: "
                  << failed << "\n";
        return 1;
    }

    std::cout << "[llm_memory_tests] all tests passed\n";
    return 0;
}