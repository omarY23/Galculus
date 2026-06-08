#include "galculus/llm_memory/MemoryAwareLLMRuntime.hpp"

#include "galculus/memory/ScopedMemoryReservation.hpp"

#include <algorithm>

namespace galculus::llm_memory {

MemoryAwareLLMRuntime::MemoryAwareLLMRuntime(
    galculus::llm::LLMRuntime& inner,
    galculus::memory::MemoryManager& memory_manager,
    std::string agent_id
)
    : inner_(inner),
      memory_manager_(memory_manager),
      agent_id_(std::move(agent_id)) {}

galculus::llm::LLMResult MemoryAwareLLMRuntime::generate(
    const galculus::llm::Prompt& prompt,
    const galculus::llm::GenerationConfig& config
) {
    const std::size_t required_bytes =
        estimate_memory_bytes(
            prompt,
            config
        );

    galculus::memory::ScopedMemoryReservation reservation(
        memory_manager_,
        agent_id_,
        required_bytes
    );

    if (!reservation.ok()) {
        galculus::llm::LLMResult result;
        result.text = "";
        result.error_message =
            "memory budget exceeded for agent: " + agent_id_;
        return result;
    }

    auto result =
        inner_.generate(
            prompt,
            config
        );

    memory_manager_.reset_scratch();

    return result;
}

std::string MemoryAwareLLMRuntime::name() const {
    return "MemoryAwareLLMRuntime(" + inner_.name() + ")";
}

std::size_t MemoryAwareLLMRuntime::estimate_memory_bytes(
    const galculus::llm::Prompt& prompt,
    const galculus::llm::GenerationConfig& config
) const {
    std::size_t prompt_bytes = 0;

    for (const auto& message : prompt.messages()) {
        prompt_bytes += message.content.size();
    }

    const std::size_t completion_bytes =
        static_cast<std::size_t>(config.max_tokens) * 8;

    const std::size_t overhead_bytes = 1024;

    return prompt_bytes + completion_bytes + overhead_bytes;
}

}