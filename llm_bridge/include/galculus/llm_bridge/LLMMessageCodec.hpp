#pragma once

#include "galculus/core/Message.hpp"

#include "galculus/llm_bridge/LLMRequest.hpp"
#include "galculus/llm_bridge/LLMResponse.hpp"

namespace galculus::llm_bridge {

class LLMMessageCodec {
public:
    static galculus::core::Message encode_request(
        const LLMRequest& request,
        const std::string& source,
        const std::string& target
    );

    static LLMRequest decode_request(
        const galculus::core::Message& message
    );

    static galculus::core::Message encode_response(
        const LLMResponse& response,
        const std::string& source,
        const std::string& target
    );

    static LLMResponse decode_response(
        const galculus::core::Message& message
    );
};

}