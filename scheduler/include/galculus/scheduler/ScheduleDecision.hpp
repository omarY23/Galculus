#pragma once

#include <cstdint>
#include <string>

namespace galculus::scheduler {

enum class ScheduleDecisionType {
    Allow,
    ReduceTokens,
    Reject,
    UseFallback
};

struct ScheduleDecision {
    ScheduleDecisionType type = ScheduleDecisionType::Reject;
    std::uint32_t allowed_completion_tokens = 0;
    std::string reason;
};

}