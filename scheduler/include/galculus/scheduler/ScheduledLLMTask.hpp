#pragma once

#include <cstdint>
#include <string>

namespace galculus::scheduler {

struct ScheduledLLMTask {
    std::string task_id;

    std::uint32_t estimated_prompt_tokens = 0;
    std::uint32_t requested_completion_tokens = 0;

    std::uint32_t deadline_ms = 0;
    std::uint8_t priority = 0;

    bool allow_truncation = true;
    bool allow_fallback = true;
};

}