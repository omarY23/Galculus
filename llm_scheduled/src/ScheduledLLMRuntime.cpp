#include "galculus/llm_scheduled/ScheduledLLMRuntime.hpp"

#include <algorithm>

namespace galculus::llm_scheduled {

ScheduledLLMRuntime::ScheduledLLMRuntime(
    galculus::llm::LLMRuntime& inner,
    galculus::scheduler::TokenScheduler& scheduler
)
    : inner_(inner),
      scheduler_(scheduler) {}

galculus::llm::LLMResult ScheduledLLMRuntime::generate(
    const galculus::llm::Prompt& prompt,
    const galculus::llm::GenerationConfig& config
) {
    galculus::scheduler::ScheduledLLMTask task;
    task.task_id = "scheduled_llm_task";
    task.estimated_prompt_tokens = estimate_prompt_tokens(prompt);
    task.requested_completion_tokens = config.max_tokens;
    task.deadline_ms = config.timeout_ms;
    task.priority = 0;
    task.allow_truncation = true;
    task.allow_fallback = true;

    auto decision = scheduler_.schedule(task);

    if (decision.type == galculus::scheduler::ScheduleDecisionType::Reject) {
        galculus::llm::LLMResult result;
        result.text = "";
        result.error_message = decision.reason;
        return result;
    }

    if (decision.type == galculus::scheduler::ScheduleDecisionType::UseFallback) {
        galculus::llm::LLMResult result;
        result.text =
            "tool=send_alert\n"
            "reason=LLM request used fallback because token budget was unavailable\n"
            "confidence=0.50\n";
        result.error_message = "";
        return result;
    }

    auto adjusted_config = config;

    if (decision.type == galculus::scheduler::ScheduleDecisionType::ReduceTokens) {
        adjusted_config.max_tokens = decision.allowed_completion_tokens;
    }

    auto result = inner_.generate(prompt, adjusted_config);

    std::uint32_t used_tokens = 0;

    if (result.prompt_tokens > 0 || result.completion_tokens > 0) {
        used_tokens =
            static_cast<std::uint32_t>(
                result.prompt_tokens + result.completion_tokens
            );
    }

    scheduler_.release(used_tokens);

    return result;
}

std::string ScheduledLLMRuntime::name() const {
    return "ScheduledLLMRuntime(" + inner_.name() + ")";
}

std::uint32_t ScheduledLLMRuntime::estimate_prompt_tokens(
    const galculus::llm::Prompt& prompt
) const {
    std::uint32_t chars = 0;

    for (const auto& message : prompt.messages()) {
        chars += static_cast<std::uint32_t>(message.content.size());
    }

    return std::max<std::uint32_t>(1, chars / 4);
}

}