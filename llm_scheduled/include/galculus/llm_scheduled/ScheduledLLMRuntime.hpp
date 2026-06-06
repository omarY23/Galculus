#pragma once

#include <string>

#include "galculus/llm/LLMRuntime.hpp"
#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/LLMResult.hpp"

#include "galculus/scheduler/TokenScheduler.hpp"

namespace galculus::llm_scheduled {

class ScheduledLLMRuntime final : public galculus::llm::LLMRuntime {
public:
    ScheduledLLMRuntime(
        galculus::llm::LLMRuntime& inner,
        galculus::scheduler::TokenScheduler& scheduler
    );

    galculus::llm::LLMResult generate(
        const galculus::llm::Prompt& prompt,
        const galculus::llm::GenerationConfig& config
    ) override;

    std::string name() const override;

private:
    std::uint32_t estimate_prompt_tokens(
        const galculus::llm::Prompt& prompt
    ) const;

private:
    galculus::llm::LLMRuntime& inner_;
    galculus::scheduler::TokenScheduler& scheduler_;
};

}