#include <galculus/llm/Prompt.hpp>
#include <galculus/llm/GenerationConfig.hpp>
#include <galculus/llm_remote/RemoteLLM.hpp>

#include <cstdint>
#include <iostream>

int main() {
    using namespace galculus::llm;
    using namespace galculus::llm_remote;

    constexpr std::uint16_t port = 39200;

    Prompt prompt;
    prompt.add_system("You are a safe embedded device planner.");
    prompt.add_user("Motion detected at the front door. Decide action.");

    GenerationConfig config;
    config.max_tokens = 32;
    config.temperature = 0.1f;

    RemoteLLM llm(
        "127.0.0.1",
        port,
        "remote_llm.example_client",
        "llm.server"
    );

    auto result =
        llm.generate(
            prompt,
            config
        );

    if (!result.ok()) {
        std::cerr << "[remote-llm-client] failed: "
                  << result.error_message << "\n";
        return 1;
    }

    std::cout << "[remote-llm-client] result: "
              << result.text << "\n";

    std::cout << "[remote-llm-client] prompt tokens: "
              << result.prompt_tokens << "\n";

    std::cout << "[remote-llm-client] completion tokens: "
              << result.completion_tokens << "\n";

    return 0;
}