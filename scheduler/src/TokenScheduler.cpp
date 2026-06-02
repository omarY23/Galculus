#include "galculus/scheduler/TokenScheduler.hpp"

#include <algorithm>

namespace galculus::scheduler {

TokenScheduler::TokenScheduler(TokenBudget budget)
    : budget_(budget) {}

ScheduleDecision TokenScheduler::schedule(const ScheduledLLMTask& task) {
    if (task.estimated_prompt_tokens > budget_.max_prompt_tokens) {
        if (task.allow_fallback) {
            return {
                ScheduleDecisionType::UseFallback,
                0,
                "prompt token estimate exceeds maximum prompt budget"
            };
        }

        return {
            ScheduleDecisionType::Reject,
            0,
            "prompt token estimate exceeds maximum prompt budget"
        };
    }

    std::uint32_t requested_completion =
        std::min(task.requested_completion_tokens, budget_.max_completion_tokens);

    if (task.requested_completion_tokens > budget_.max_completion_tokens &&
        !task.allow_truncation) {
        if (task.allow_fallback) {
            return {
                ScheduleDecisionType::UseFallback,
                0,
                "requested completion exceeds maximum completion budget"
            };
        }

        return {
            ScheduleDecisionType::Reject,
            0,
            "requested completion exceeds maximum completion budget"
        };
    }

    const std::uint32_t total_requested =
        task.estimated_prompt_tokens + requested_completion;

    if (total_requested <= budget_.remaining_tokens) {
        budget_.remaining_tokens -= total_requested;

        if (requested_completion < task.requested_completion_tokens) {
            return {
                ScheduleDecisionType::ReduceTokens,
                requested_completion,
                "completion tokens reduced to max completion budget"
            };
        }

        return {
            ScheduleDecisionType::Allow,
            requested_completion,
            "task allowed"
        };
    }

    if (budget_.remaining_tokens <= task.estimated_prompt_tokens) {
        if (task.allow_fallback) {
            return {
                ScheduleDecisionType::UseFallback,
                0,
                "not enough remaining budget for prompt and completion"
            };
        }

        return {
            ScheduleDecisionType::Reject,
            0,
            "not enough remaining budget for prompt and completion"
        };
    }

    const std::uint32_t available_completion =
        budget_.remaining_tokens - task.estimated_prompt_tokens;

    if (task.allow_truncation && available_completion > 0) {
        budget_.remaining_tokens = 0;

        return {
            ScheduleDecisionType::ReduceTokens,
            available_completion,
            "completion tokens reduced due to remaining token budget"
        };
    }

    if (task.allow_fallback) {
        return {
            ScheduleDecisionType::UseFallback,
            0,
            "token budget too low and truncation disabled"
        };
    }

    return {
        ScheduleDecisionType::Reject,
        0,
        "token budget too low and truncation disabled"
    };
}

void TokenScheduler::release(std::uint32_t used_tokens) {
    budget_.remaining_tokens =
        std::min(
            budget_.remaining_tokens + used_tokens,
            budget_.max_prompt_tokens + budget_.max_completion_tokens
        );
}

TokenBudget TokenScheduler::budget() const {
    return budget_;
}

}