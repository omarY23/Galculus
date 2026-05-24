#include <galculus/core/agent.hpp>
#include <galculus/core/runtime.hpp>
#include <galculus/core/in_memory_transport.hpp>

#include <cassert>
#include <memory>
#include <string>

using namespace galculus::core;

class TestAgent final : public Agent {
public:
    explicit TestAgent(std::string agent_name)
        : agent_name_(std::move(agent_name)) {}

    bool started{false};
    bool stopped{false};
    int received_count{0};
    std::string last_payload;
    MessageId last_message_id{0};

    std::string name() const override {
        return agent_name_;
    }

    void on_start() override {
        started = true;
    }

    void on_message(const Message& message) override {
        received_count++;
        last_payload = message.event.payload_as_string();
        last_message_id = message.id;
    }

    void on_stop() override {
        stopped = true;
    }

private:
    std::string agent_name_;
};

int main() {

    InMemoryTransport transport;
    AgentRuntime runtime(transport);

    auto agent = std::make_unique<TestAgent>("test");
    TestAgent* raw_agent = agent.get();

    auto result = runtime.register_agent(std::move(agent));
    assert(result.ok);
    assert(runtime.has_agent("test"));
    assert(runtime.agent_count() == 1);

    runtime.start();
    assert(runtime.is_running());
    assert(raw_agent->started);

    result = runtime.publish_event(
        "tester",
        "test",
        Event::text("ping", "hello")
    );

    assert(result.ok);

    result = runtime.run_once();
    assert(result.ok);

    assert(raw_agent->received_count == 1);
    assert(raw_agent->last_payload == "hello");
    assert(raw_agent->last_message_id == 1);

    result = runtime.publish_event(
        "tester",
        "missing",
        Event::text("ping", "fail")
    );

    assert(result.ok);

    result = runtime.run_once();
    assert(!result.ok);
    assert(result.code == ErrorCode::AgentNotFound);

    auto agent2 = std::make_unique<TestAgent>("second");
    TestAgent* raw_agent2 = agent2.get();

    result = runtime.register_agent(std::move(agent2));
    assert(result.ok);

    result = runtime.broadcast_event(
        "tester",
        Event::text("broadcast", "all")
    );

    assert(result.ok);

    runtime.drain();

    assert(raw_agent->received_count == 2);
    assert(raw_agent2->received_count == 1);

    result = runtime.unregister_agent("second");
    assert(result.ok);
    assert(!runtime.has_agent("second"));
    assert(raw_agent2->stopped);

    runtime.stop();

    assert(!runtime.is_running());
    assert(raw_agent->stopped);

    return 0;
}