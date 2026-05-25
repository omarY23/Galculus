#pragma once
#include "LLMResult.hpp"
#include "Prompt.hpp"
#include "GenerationConfig.hpp"
#include "LLMRuntime.hpp"

namespace galculus::llm {
    class MockLLM final : public LLMRuntime {
    public:
        explicit MockLLM(std::string fixed_response = "mock response");
    
        LLMResult generate(
            const Prompt& prompt,
            const GenerationConfig& config
        ) override;
    
        std::string name() const override;
    
    private:
        std::string fixed_response_;
    };

} // namespace galculus::llm