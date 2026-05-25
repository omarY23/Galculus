#include "galculus/llm_bridge/LLMMessageCodec.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace galculus::llm;
using namespace galculus::llm_bridge;

static void test_encode_decode_llm_request() {
    LLMRequest request;
    request.request_id = "req-1";
    request.prompt.add_system("You are a safe embedded planner.");
    request.prompt.add_user("Motion detected at the door.");

    request.config.max_tokens = 32;
    request.config.temperature = 0.1f;
    request.config.top_p = 0.9f;
    request.config.seed = 42;
    request.config.timeout_ms = 500;
    request.config.stream = false;

    auto message = LLMMessageCodec::encode_request(
        request,
        "client.node",
        "server.llm"
    );

    assert(message.event.name == "LLM_REQUEST");
    assert(message.source == "client.node");
    assert(message.target == "server.llm");

    auto decoded = LLMMessageCodec::decode_request(message);

    assert(decoded.request_id == "req-1");
    assert(decoded.prompt.messages().size() == 2);
    assert(decoded.prompt.messages()[0].content == "You are a safe embedded planner.");
    assert(decoded.prompt.messages()[1].content == "Motion detected at the door.");

    assert(decoded.config.max_tokens == 32);
    assert(decoded.config.temperature == 0.1f);
    assert(decoded.config.top_p == 0.9f);
    assert(decoded.config.seed == 42);
    assert(decoded.config.timeout_ms == 500);
    assert(decoded.config.stream == false);
}

static void test_encode_decode_llm_response() {
    LLMResponse response;
    response.request_id = "req-1";
    response.result.status = LLMStatus::Ok;
    response.result.text = "turn_on_light";
    response.result.prompt_tokens = 7;
    response.result.completion_tokens = 1;
    response.result.elapsed_ms = 12;
    response.result.error_message = "";

    auto message = LLMMessageCodec::encode_response(
        response,
        "server.llm",
        "client.node"
    );

    assert(message.event.name == "LLM_RESPONSE");
    assert(message.source == "server.llm");
    assert(message.target == "client.node");

    auto decoded = LLMMessageCodec::decode_response(message);

    assert(decoded.request_id == "req-1");
    assert(decoded.result.status == LLMStatus::Ok);
    assert(decoded.result.text == "turn_on_light");
    assert(decoded.result.prompt_tokens == 7);
    assert(decoded.result.completion_tokens == 1);
    assert(decoded.result.elapsed_ms == 12);
    assert(decoded.result.error_message == "");
}

int main() {
    test_encode_decode_llm_request();
    test_encode_decode_llm_response();

    std::cout << "All LLM message codec tests passed.\n";
    return 0;
}