#include <galculus/llm_bridge/LLMMessageCodec.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace galculus::llm_bridge {

using galculus::core::Message;
using galculus::llm::GenerationConfig;
using galculus::llm::LLMResult;
using galculus::llm::LLMStatus;
using galculus::llm::Prompt;
using galculus::llm::PromptRole;

namespace {

class Writer {
public:
    void u8(std::uint8_t value) {
        data_.push_back(value);
    }

    void u32(std::uint32_t value) {
        data_.push_back(static_cast<std::uint8_t>((value >> 0) & 0xff));
        data_.push_back(static_cast<std::uint8_t>((value >> 8) & 0xff));
        data_.push_back(static_cast<std::uint8_t>((value >> 16) & 0xff));
        data_.push_back(static_cast<std::uint8_t>((value >> 24) & 0xff));
    }

    void f32(float value) {
        static_assert(sizeof(float) == sizeof(std::uint32_t));

        std::uint32_t raw = 0;
        std::memcpy(&raw, &value, sizeof(float));
        u32(raw);
    }

    void str(const std::string& value) {
        u32(static_cast<std::uint32_t>(value.size()));

        data_.insert(
            data_.end(),
            value.begin(),
            value.end()
        );
    }

    const std::vector<std::uint8_t>& data() const {
        return data_;
    }

private:
    std::vector<std::uint8_t> data_;
};

class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& data)
        : data_(data) {}

    std::uint8_t u8() {
        require(1);
        return data_[offset_++];
    }

    std::uint32_t u32() {
        require(4);

        std::uint32_t value = 0;
        value |= static_cast<std::uint32_t>(data_[offset_ + 0]) << 0;
        value |= static_cast<std::uint32_t>(data_[offset_ + 1]) << 8;
        value |= static_cast<std::uint32_t>(data_[offset_ + 2]) << 16;
        value |= static_cast<std::uint32_t>(data_[offset_ + 3]) << 24;

        offset_ += 4;
        return value;
    }

    float f32() {
        std::uint32_t raw = u32();

        float value = 0.0f;
        std::memcpy(&value, &raw, sizeof(float));
        return value;
    }

    std::string str() {
        const auto size = u32();
        require(size);

        std::string value(
            data_.begin() + static_cast<std::ptrdiff_t>(offset_),
            data_.begin() + static_cast<std::ptrdiff_t>(offset_ + size)
        );

        offset_ += size;
        return value;
    }

private:
    void require(std::size_t count) const {
        if (offset_ + count > data_.size()) {
            throw std::runtime_error("LLMMessageCodec: truncated payload");
        }
    }

    const std::vector<std::uint8_t>& data_;
    std::size_t offset_{0};
};

std::uint8_t role_to_u8(PromptRole role) {
    switch (role) {
        case PromptRole::System: return 0;
        case PromptRole::User: return 1;
        case PromptRole::Assistant: return 2;
        case PromptRole::Tool: return 3;
    }

    return 255;
}

PromptRole u8_to_role(std::uint8_t value) {
    switch (value) {
        case 0: return PromptRole::System;
        case 1: return PromptRole::User;
        case 2: return PromptRole::Assistant;
        case 3: return PromptRole::Tool;
        default:
            throw std::runtime_error("LLMMessageCodec: invalid prompt role");
    }
}

std::uint8_t status_to_u8(LLMStatus status) {
    switch (status) {
        case LLMStatus::Ok: return 0;
        case LLMStatus::Timeout: return 1;
        case LLMStatus::Cancelled: return 2;
        case LLMStatus::InvalidPrompt: return 3;
        case LLMStatus::BackendError: return 4;
    }

    return 4;
}

LLMStatus u8_to_status(std::uint8_t value) {
    switch (value) {
        case 0: return LLMStatus::Ok;
        case 1: return LLMStatus::Timeout;
        case 2: return LLMStatus::Cancelled;
        case 3: return LLMStatus::InvalidPrompt;
        case 4: return LLMStatus::BackendError;
        default:
            return LLMStatus::BackendError;
    }
}

void add_prompt_message(
    Prompt& prompt,
    PromptRole role,
    const std::string& content
) {
    switch (role) {
        case PromptRole::System:
            prompt.add_system(content);
            break;
        case PromptRole::User:
            prompt.add_user(content);
            break;
        case PromptRole::Assistant:
            prompt.add_assistant(content);
            break;
        case PromptRole::Tool:
            prompt.add_tool(content);
            break;
    }
}

} // namespace

Message LLMMessageCodec::encode_request(
    const LLMRequest& request,
    const std::string& source,
    const std::string& target
) {
    Writer writer;

    writer.str("GLM_REQ_V1");

    writer.str(request.request_id);

    writer.u32(request.config.max_tokens);
    writer.f32(request.config.temperature);
    writer.f32(request.config.top_p);
    writer.u32(request.config.seed);
    writer.u32(request.config.timeout_ms);
    writer.u8(request.config.stream ? 1 : 0);

    writer.u32(
        static_cast<std::uint32_t>(
            request.prompt.messages().size()
        )
    );

    for (const auto& msg : request.prompt.messages()) {
        writer.u8(role_to_u8(msg.role));
        writer.str(msg.content);
    }

    Message message;
    message.id = 0;
    message.source = source;
    message.target = target;
    message.event.name = "LLM_REQUEST";
    message.event.payload = writer.data();

    return message;
}

LLMRequest LLMMessageCodec::decode_request(
    const Message& message
) {
    if (message.event.name != "LLM_REQUEST") {
        throw std::runtime_error("Message is not LLM_REQUEST");
    }

    Reader reader(message.event.payload);

    const auto magic = reader.str();

    if (magic != "GLM_REQ_V1") {
        throw std::runtime_error("Invalid LLM request payload");
    }

    LLMRequest request;

    request.request_id = reader.str();

    request.config.max_tokens = reader.u32();
    request.config.temperature = reader.f32();
    request.config.top_p = reader.f32();
    request.config.seed = reader.u32();
    request.config.timeout_ms = reader.u32();
    request.config.stream = reader.u8() != 0;

    const auto message_count = reader.u32();

    for (std::uint32_t i = 0; i < message_count; ++i) {
        const auto role = u8_to_role(reader.u8());
        const auto content = reader.str();

        add_prompt_message(
            request.prompt,
            role,
            content
        );
    }

    return request;
}

Message LLMMessageCodec::encode_response(
    const LLMResponse& response,
    const std::string& source,
    const std::string& target
) {
    Writer writer;

    writer.str("GLM_RES_V1");

    writer.str(response.request_id);

    writer.u8(status_to_u8(response.result.status));
    writer.str(response.result.text);
    writer.u32(response.result.prompt_tokens);
    writer.u32(response.result.completion_tokens);
    writer.u32(response.result.elapsed_ms);
    writer.str(response.result.error_message);

    Message message;
    message.id = 0;
    message.source = source;
    message.target = target;
    message.event.name = "LLM_RESPONSE";
    message.event.payload = writer.data();

    return message;
}

LLMResponse LLMMessageCodec::decode_response(
    const Message& message
) {
    if (message.event.name != "LLM_RESPONSE") {
        throw std::runtime_error("Message is not LLM_RESPONSE");
    }

    Reader reader(message.event.payload);

    const auto magic = reader.str();

    if (magic != "GLM_RES_V1") {
        throw std::runtime_error("Invalid LLM response payload");
    }

    LLMResponse response;

    response.request_id = reader.str();

    response.result.status = u8_to_status(reader.u8());
    response.result.text = reader.str();
    response.result.prompt_tokens = reader.u32();
    response.result.completion_tokens = reader.u32();
    response.result.elapsed_ms = reader.u32();
    response.result.error_message = reader.str();

    return response;
}

} // namespace galculus::llm_bridge