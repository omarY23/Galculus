#pragma once
#include "LLMResult.hpp"
#include "Prompt.hpp"
#include "GenerationConfig.hpp"
#include <string>

namespace galculus::llm {


class LLMRuntime {
public:
    virtual ~LLMRuntime() = default;

    virtual LLMResult generate(
        const Prompt& prompt,
        const GenerationConfig& config
    ) = 0;

    virtual std::string name() const = 0;
};

} // namespace galculus::llm