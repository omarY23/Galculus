#include "galculus/llm_bridge/LLMMessageCodec.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::vector<uint8_t> string_to_bytes(
    const std::string& s
) {
    return std::vector<uint8_t>(
        s.begin(),
        s.end()
    );
}

std::string bytes_to_string(
    const std::vector<uint8_t>& data
) {
    return std::string(
        data.begin(),
        data.end()
    );
}

}

namespace galculus::llm_bridge {

using galculus::core::Message;
using galculus::llm::GenerationConfig;
using galculus::llm::LLMResult;
using galculus::llm::LLMStatus;
using galculus::llm::Prompt;
using galculus::llm::PromptRole;

namespace {

std::string role_to_string(PromptRole role) {
    switch (role) {
        case PromptRole::System: return "system";
        case PromptRole::User: return "user";
        case PromptRole::Assistant: return "assistant";
        case PromptRole::Tool: return "tool";
    }

    return "unknown";
}

PromptRole string_to_role(const std::string& role) {
    if (role == "system") return PromptRole::System;
    if (role == "user") return PromptRole::User;
    if (role == "assistant") return PromptRole::Assistant;
    if (role == "tool") return PromptRole::Tool;

    throw std::runtime_error("Unknown prompt role: " + role);
}

std::string status_to_string(LLMStatus status) {
    switch (status) {
        case LLMStatus::Ok: return "ok";
        case LLMStatus::Timeout: return "timeout";
        case LLMStatus::Cancelled: return "cancelled";
        case LLMStatus::InvalidPrompt: return "invalid_prompt";
        case LLMStatus::BackendError: return "backend_error";
    }

    return "backend_error";
}

LLMStatus string_to_status(const std::string& status) {
    if (status == "ok") return LLMStatus::Ok;
    if (status == "timeout") return LLMStatus::Timeout;
    if (status == "cancelled") return LLMStatus::Cancelled;
    if (status == "invalid_prompt") return LLMStatus::InvalidPrompt;
    if (status == "backend_error") return LLMStatus::BackendError;

    return LLMStatus::BackendError;
}

std::string read_value(const std::string& line, const std::string& key) {
    const std::string prefix = key + "=";

    if (line.rfind(prefix, 0) != 0) {
        throw std::runtime_error("Expected key: " + key);
    }

    return line.substr(prefix.size());
}

} // namespace

Message LLMMessageCodec::encode_request(
    const LLMRequest& request,
    const std::string& source,
    const std::string& target
) {
    std::ostringstream payload;

    payload << "request_id=" << request.request_id << '\n';

    payload << "max_tokens=" << request.config.max_tokens << '\n';
    payload << "temperature=" << request.config.temperature << '\n';
    payload << "top_p=" << request.config.top_p << '\n';
    payload << "seed=" << request.config.seed << '\n';
    payload << "timeout_ms=" << request.config.timeout_ms << '\n';
    payload << "stream=" << (request.config.stream ? 1 : 0) << '\n';

    payload << "messages=" << request.prompt.messages().size() << '\n';

    for (const auto& msg : request.prompt.messages()) {
        payload << "role=" << role_to_string(msg.role) << '\n';
        payload << "content=" << msg.content << '\n';
    }

    Message message;
    message.id = 0;
    message.source = source;
    message.target = target;
    message.event.name = "LLM_REQUEST";
    message.event.payload =
        string_to_bytes(
            payload.str()
        );

    return message;
}

LLMRequest LLMMessageCodec::decode_request(
    const Message& message
) {
    if (message.event.name != "LLM_REQUEST") {
        throw std::runtime_error("Message is not LLM_REQUEST");
    }

    std::istringstream input(bytes_to_string(
        message.event.payload
    ));
    std::string line;

    LLMRequest request;

    std::getline(input, line);
    request.request_id = read_value(line, "request_id");

    std::getline(input, line);
    request.config.max_tokens = static_cast<uint32_t>(
        std::stoul(read_value(line, "max_tokens"))
    );

    std::getline(input, line);
    request.config.temperature = std::stof(read_value(line, "temperature"));

    std::getline(input, line);
    request.config.top_p = std::stof(read_value(line, "top_p"));

    std::getline(input, line);
    request.config.seed = static_cast<uint32_t>(
        std::stoul(read_value(line, "seed"))
    );

    std::getline(input, line);
    request.config.timeout_ms = static_cast<uint32_t>(
        std::stoul(read_value(line, "timeout_ms"))
    );

    std::getline(input, line);
    request.config.stream = std::stoi(read_value(line, "stream")) != 0;

    std::getline(input, line);
    const auto count = static_cast<size_t>(
        std::stoul(read_value(line, "messages"))
    );

    for (size_t i = 0; i < count; ++i) {
        std::getline(input, line);
        const auto role = string_to_role(read_value(line, "role"));

        std::getline(input, line);
        const auto content = read_value(line, "content");

        switch (role) {
            case PromptRole::System:
                request.prompt.add_system(content);
                break;
            case PromptRole::User:
                request.prompt.add_user(content);
                break;
            case PromptRole::Assistant:
                request.prompt.add_assistant(content);
                break;
            case PromptRole::Tool:
                request.prompt.add_tool(content);
                break;
        }
    }

    return request;
}

Message LLMMessageCodec::encode_response(
    const LLMResponse& response,
    const std::string& source,
    const std::string& target
) {
    std::ostringstream payload;

    payload << "request_id=" << response.request_id << '\n';
    payload << "status=" << status_to_string(response.result.status) << '\n';
    payload << "text=" << response.result.text << '\n';
    payload << "prompt_tokens=" << response.result.prompt_tokens << '\n';
    payload << "completion_tokens=" << response.result.completion_tokens << '\n';
    payload << "elapsed_ms=" << response.result.elapsed_ms << '\n';
    payload << "error_message=" << response.result.error_message << '\n';

    Message message;
    message.id = 0;
    message.source = source;
    message.target = target;
    message.event.name = "LLM_RESPONSE";
    message.event.payload =
        string_to_bytes(
            payload.str()
        );
    return message;
}

LLMResponse LLMMessageCodec::decode_response(
    const Message& message
) {
    if (message.event.name != "LLM_RESPONSE") {
        throw std::runtime_error("Message is not LLM_RESPONSE");
    }

    std::istringstream input(bytes_to_string(
        message.event.payload
    ));
    std::string line;

    LLMResponse response;

    std::getline(input, line);
    response.request_id = read_value(line, "request_id");

    std::getline(input, line);
    response.result.status = string_to_status(read_value(line, "status"));

    std::getline(input, line);
    response.result.text = read_value(line, "text");

    std::getline(input, line);
    response.result.prompt_tokens = static_cast<uint32_t>(
        std::stoul(read_value(line, "prompt_tokens"))
    );

    std::getline(input, line);
    response.result.completion_tokens = static_cast<uint32_t>(
        std::stoul(read_value(line, "completion_tokens"))
    );

    std::getline(input, line);
    response.result.elapsed_ms = static_cast<uint32_t>(
        std::stoul(read_value(line, "elapsed_ms"))
    );

    std::getline(input, line);
    response.result.error_message = read_value(line, "error_message");

    return response;
}

} // namespace galculus::llm_bridge