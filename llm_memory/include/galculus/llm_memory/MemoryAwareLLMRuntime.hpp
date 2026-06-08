#pragma once

#include <cstddef>
#include <string>

#include "galculus/llm/LLMRuntime.hpp"
#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/LLMResult.hpp"

#include "galculus/memory/MemoryManager.hpp"

namespace galculus::llm_memory {

class MemoryAwareLLMRuntime final : public galculus::llm::LLMRuntime {
public:
    MemoryAwareLLMRuntime(
        galculus::llm::LLMRuntime& inner,
        galculus::memory::MemoryManager& memory_manager,
        std::string agent_id
    );

    galculus::llm::LLMResult generate(
        const galculus::llm::Prompt& prompt,
        const galculus::llm::GenerationConfig& config
    ) override;

    std::string name() const override;

private:
    std::size_t estimate_memory_bytes(
        const galculus::llm::Prompt& prompt,
        const galculus::llm::GenerationConfig& config
    ) const;

private:
    galculus::llm::LLMRuntime& inner_;
    galculus::memory::MemoryManager& memory_manager_;
    std::string agent_id_;
};

}