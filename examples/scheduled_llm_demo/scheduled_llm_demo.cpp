#include <iostream>

#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/MockLLM.hpp"

#include "galculus/scheduler/TokenBudget.hpp"
#include "galculus/scheduler/TokenScheduler.hpp"

#include "galculus/llm_scheduled/ScheduledLLMRuntime.hpp"

int main() {
    galculus::llm::MockLLM mock_llm;

    galculus::scheduler::TokenBudget budget;
    budget.max_prompt_tokens = 128;
    budget.max_completion_tokens = 16;
    budget.remaining_tokens = 144;

    galculus::scheduler::TokenScheduler scheduler(budget);

    galculus::llm_scheduled::ScheduledLLMRuntime scheduled_llm(
        mock_llm,
        scheduler
    );

    galculus::llm::Prompt prompt;
    prompt.add_system("You are a safe embedded planner.");
    prompt.add_user("Motion detected near the door. Decide what tool to call.");

    galculus::llm::GenerationConfig config;
    config.max_tokens = 64;
    config.temperature = 0.1f;
    config.top_p = 0.9f;
    config.timeout_ms = 1000;
    config.stream = false;

    auto result = scheduled_llm.generate(prompt, config);
    std::cout << "[scheduled demo] backend: " << scheduled_llm.name() << "\n";
    std::cout << "[scheduled demo] error: " << result.error_message << "\n";
    std::cout << "[scheduled demo] text:\n" << result.text << "\n";

    return result.error_message.empty() ? 0 : 1;
}