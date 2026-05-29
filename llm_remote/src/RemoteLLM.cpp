#include <galculus/llm_remote/RemoteLLM.hpp>

#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <galculus/llm_bridge/LLMMessageCodec.hpp>

#include <chrono>
#include <sstream>
#include <utility>

namespace galculus::llm_remote {

RemoteLLM::RemoteLLM(
    std::string host,
    std::uint16_t port,
    std::string source,
    std::string target
)
    : host_(std::move(host)),
      port_(port),
      source_(std::move(source)),
      target_(std::move(target)) {}

std::string RemoteLLM::name() const {
    return "RemoteLLM";
}

std::string RemoteLLM::next_request_id() {
    ++request_counter_;

    std::ostringstream out;
    out << "remote-llm-req-" << request_counter_;

    return out.str();
}

galculus::llm::LLMResult RemoteLLM::generate(
    const galculus::llm::Prompt& prompt,
    const galculus::llm::GenerationConfig& config
) {
    using namespace galculus::core;
    using namespace galculus::llm_bridge;

    const auto start = std::chrono::steady_clock::now();

    auto byte_transport =
        TcpByteTransport::connect_to(
            host_,
            port_
        );

    if (!byte_transport) {
        galculus::llm::LLMResult result;
        result.status = galculus::llm::LLMStatus::BackendError;
        result.error_message = "Failed to connect to remote LLM server";
        return result;
    }

    FramedMessageTransport transport(*byte_transport);

    LLMRequest request;
    request.request_id = next_request_id();
    request.prompt = prompt;
    request.config = config;

    auto message =
        LLMMessageCodec::encode_request(
            request,
            source_,
            target_
        );

    if (!transport.send(message)) {
        galculus::llm::LLMResult result;
        result.status = galculus::llm::LLMStatus::BackendError;
        result.error_message = "Failed to send LLM_REQUEST";
        return result;
    }

    auto received = transport.receive();

    if (!received.has_value()) {
        galculus::llm::LLMResult result;
        result.status = galculus::llm::LLMStatus::Timeout;
        result.error_message = "Failed to receive LLM_RESPONSE";
        return result;
    }

    if (received->event.name != "LLM_RESPONSE") {
        galculus::llm::LLMResult result;
        result.status = galculus::llm::LLMStatus::BackendError;
        result.error_message =
            "Received unexpected event: " + received->event.name;
        return result;
    }

    auto response =
        LLMMessageCodec::decode_response(
            *received
        );

    auto result = response.result;

    const auto end = std::chrono::steady_clock::now();

    result.elapsed_ms = static_cast<std::uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start
        ).count()
    );

    return result;
}

}