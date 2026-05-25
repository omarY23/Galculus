#pragma once

#include <string>

#include "galculus/llm/LLMResult.hpp"

namespace galculus::llm_bridge {

struct LLMResponse {
    std::string request_id;
    galculus::llm::LLMResult result;
};

}