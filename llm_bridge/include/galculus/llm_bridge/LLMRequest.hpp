#pragma once

#include <string>

#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"

namespace galculus::llm_bridge {

struct LLMRequest {
    std::string request_id;
    galculus::llm::Prompt prompt;
    galculus::llm::GenerationConfig config;
};

}