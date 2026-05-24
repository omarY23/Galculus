#include <galculus/core/agent.hpp>
#include <galculus/core/in_memory_transport.hpp>
#include <galculus/core/runtime.hpp>

#include <iostream>
#include <memory>

using namespace galculus::core;

class LoggerAgent final : public Agent {
public:
    std::string name() const override {
        return "logger";
    }

    void on_start() override {
        std::cout << "[logger] started\n";
    }

    void on_message(const Message& message) override {
        std::cout << "[logger] received: "
                  << message.event.name
                  << " payload="
                  << message.event.payload_as_string()
                  << " id="
                  << message.id
                  << '\n';
    }

    void on_stop() override {
        std::cout << "[logger] stopped\n";
    }
};

int main() {
    InMemoryTransport transport;
    AgentRuntime runtime(transport);

    auto result = runtime.register_agent(std::make_unique<LoggerAgent>());

    if (!result.ok) {
        std::cerr << result.message << '\n';
        return 1;
    }

    runtime.start();

    runtime.publish_event(
        "system",
        "logger",
        Event::text("hello", "Galculus core runtime v0.1 online")
    );

    runtime.drain();

    runtime.stop();

    return 0;
}