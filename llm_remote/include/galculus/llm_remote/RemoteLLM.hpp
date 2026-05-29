#pragma once

#include <cstdint>
#include <string>

#include <galculus/llm/LLMRuntime.hpp>

namespace galculus::llm_remote {

class RemoteLLM final : public galculus::llm::LLMRuntime {
public:
    RemoteLLM(
        std::string host,
        std::uint16_t port,
        std::string source = "remote_llm.client",
        std::string target = "remote_llm.server"
    );

    galculus::llm::LLMResult generate(
        const galculus::llm::Prompt& prompt,
        const galculus::llm::GenerationConfig& config
    ) override;

    std::string name() const override;

private:
    std::string host_;
    std::uint16_t port_;
    std::string source_;
    std::string target_;

    std::string next_request_id();
    std::uint64_t request_counter_{0};
};

}