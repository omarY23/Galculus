#pragma once

#include <cstdint>

namespace galculus::scheduler {

struct TokenBudget {
    std::uint32_t max_prompt_tokens = 0;
    std::uint32_t max_completion_tokens = 0;
    std::uint32_t remaining_tokens = 0;
};

}