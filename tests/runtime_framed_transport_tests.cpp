#include <galculus/core/agent.hpp>
#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/in_memory_byte_transport.hpp>
#include <galculus/core/runtime.hpp>

#include <cassert>
#include <memory>
#include <string>

using namespace galculus::core;

class FramedRuntimeTestAgent final : public Agent {
public:
    bool received{false};
    std::string payload;

    std::string name() const override {
        return "agent";
    }

    void on_message(const Message& message) override {
        received = true;
        payload = message.event.payload_as_string();
    }
};

int main() {
    InMemoryByteTransport byte_transport;
    FramedMessageTransport message_transport(byte_transport);
    AgentRuntime runtime(message_transport);

    auto agent = std::make_unique<FramedRuntimeTestAgent>();
    auto* raw_agent = agent.get();

    auto result = runtime.register_agent(std::move(agent));
    assert(result.ok);

    runtime.start();

    result = runtime.publish_event(
        "tester",
        "agent",
        Event::text("ping", "through framed transport")
    );

    assert(result.ok);

    result = runtime.run_once();

    assert(result.ok);
    assert(raw_agent->received);
    assert(raw_agent->payload == "through framed transport");

    runtime.stop();

    return 0;
}