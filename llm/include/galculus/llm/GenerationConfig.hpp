#pragma once
namespace galculus::llm {
    struct GenerationConfig {
        uint32_t max_tokens = 128;
        float temperature = 0.2f;
        float top_p = 1.0f;
        uint32_t seed = 0;
    
        uint32_t timeout_ms = 1000;
        bool stream = false;
    };

} // namespace galculus::llm