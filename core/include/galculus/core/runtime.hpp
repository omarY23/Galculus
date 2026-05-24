#pragma once

#include <galculus/core/agent.hpp>
#include <galculus/core/in_memory_transport.hpp>
#include <galculus/core/result.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace galculus::core {

class AgentRuntime {
public:
    AgentRuntime();

    Result register_agent(std::unique_ptr<Agent> agent);

    Result unregister_agent(const std::string& name);

    Result send(const Message& message);

    Result publish_event(
        const std::string& source,
        const std::string& target,
        Event event
    );

    Result broadcast_event(
        const std::string& source,
        Event event
    );

    void start();

    void stop();

    Result run_once();

    void drain();

    bool is_running() const;

    bool has_agent(const std::string& name) const;

    std::size_t agent_count() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Agent>> agents_;
    InMemoryTransport transport_;
    bool running_{false};
    MessageId next_message_id_{1};

    MessageId next_message_id();

    Result dispatch(const Message& message);
};

} // namespace galculus::core