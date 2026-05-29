#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <galculus/llm/prompt.hpp>
#include <galculus/llm/GenerationConfig.hpp>
#include <galculus/llm_bridge/LLMMessageCodec.hpp>

#include <cstdint>
#include <iostream>

using namespace galculus::core;
using namespace galculus::llm;
using namespace galculus::llm_bridge;

int main() {
    constexpr std::uint16_t port = 39200;

    std::cout << "[llm-client] connecting to 127.0.0.1:"
              << port << "\n";

    auto byte_transport =
        TcpByteTransport::connect_to(
            "127.0.0.1",
            port
        );

    if (!byte_transport) {
        std::cerr << "[llm-client] failed to connect\n";
        return 1;
    }

    FramedMessageTransport transport(*byte_transport);

    Prompt prompt;
    prompt.add_system("You are a safe embedded device planner.");
    prompt.add_user("Motion detected at the front door. Decide action.");

    GenerationConfig config;
    config.max_tokens = 32;
    config.temperature = 0.1f;
    config.top_p = 0.9f;
    config.timeout_ms = 1000;
    config.stream = false;

    LLMRequest request;
    request.request_id = "req-llm-1";
    request.prompt = prompt;
    request.config = config;

    Message request_message =
        LLMMessageCodec::encode_request(
            request,
            "llm.client",
            "llm.server"
        );

    std::cout << "[llm-client] sending LLM_REQUEST\n";

    if (!transport.send(request_message)) {
        std::cerr << "[llm-client] failed to send request\n";
        return 1;
    }

    auto received = transport.receive();

    if (!received.has_value()) {
        std::cerr << "[llm-client] failed to receive response\n";
        return 1;
    }

    std::cout << "[llm-client] received event: "
              << received->event.name << "\n";

    if (received->event.name != "LLM_RESPONSE") {
        std::cerr << "[llm-client] expected LLM_RESPONSE\n";
        return 1;
    }

    LLMResponse response =
        LLMMessageCodec::decode_response(*received);

    std::cout << "[llm-client] request id: "
              << response.request_id << "\n";

    std::cout << "[llm-client] result: "
              << response.result.text << "\n";

    std::cout << "[llm-client] prompt tokens: "
              << response.result.prompt_tokens << "\n";

    std::cout << "[llm-client] completion tokens: "
              << response.result.completion_tokens << "\n";

    return 0;
}