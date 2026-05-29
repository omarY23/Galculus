#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <galculus/llm/MockLLM.hpp>
#include <galculus/llm_bridge/LLMMessageCodec.hpp>

#include <cstdint>
#include <iostream>

using namespace galculus::core;
using namespace galculus::llm;
using namespace galculus::llm_bridge;

int main() {
    constexpr std::uint16_t port = 39200;

    std::cout << "[llm-server] listening on port " << port << "\n";

    auto byte_transport = TcpByteTransport::listen_once(port);

    if (!byte_transport) {
        std::cerr << "[llm-server] failed to listen\n";
        return 1;
    }

    std::cout << "[llm-server] client connected\n";

    FramedMessageTransport transport(*byte_transport);

    auto received = transport.receive();

    if (!received.has_value()) {
        std::cerr << "[llm-server] failed to receive message\n";
        return 1;
    }

    std::cout << "[llm-server] received event: "
              << received->event.name << "\n";

    if (received->event.name != "LLM_REQUEST") {
        std::cerr << "[llm-server] expected LLM_REQUEST\n";
        return 1;
    }

    LLMRequest request =
        LLMMessageCodec::decode_request(*received);

    std::cout << "[llm-server] request id: "
              << request.request_id << "\n";

    std::cout << "[llm-server] prompt:\n"
              << request.prompt.to_string() << "\n";

    MockLLM llm("turn_on_light");

    LLMResult result =
        llm.generate(
            request.prompt,
            request.config
        );

    LLMResponse response;
    response.request_id = request.request_id;
    response.result = result;

    Message response_message =
        LLMMessageCodec::encode_response(
            response,
            "llm.server",
            received->source
        );

    if (!transport.send(response_message)) {
        std::cerr << "[llm-server] failed to send response\n";
        return 1;
    }

    std::cout << "[llm-server] response sent: "
              << result.text << "\n";

    return 0;
}