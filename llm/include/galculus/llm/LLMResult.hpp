#pragma once
#include <string>

namespace galculus::llm {
    
    enum class LLMStatus {
        Ok,
        Timeout,
        Cancelled,
        InvalidPrompt,
        BackendError
    };
    
    struct LLMResult {
        LLMStatus status = LLMStatus::Ok;
        std::string text;
    
        uint32_t prompt_tokens = 0;
        uint32_t completion_tokens = 0;
        uint32_t elapsed_ms = 0;
    
        std::string error_message;
    
        bool ok() const {
            return status == LLMStatus::Ok;
        }
    };
} // namespace galculus::llm