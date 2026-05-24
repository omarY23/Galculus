#include <galculus/core/runtime.hpp>

#include <iostream>

namespace galculus::core {

AgentRuntime::AgentRuntime(Transport& transport)
    : transport_(transport) {}
    
Result AgentRuntime::register_agent(std::unique_ptr<Agent> agent) {
    if (!agent) {
        return Result::failure(
            ErrorCode::InvalidArgument,
            "cannot register null agent"
        );
    }

    const std::string agent_name = agent->name();

    if (agent_name.empty()) {
        return Result::failure(
            ErrorCode::InvalidArgument,
            "agent name cannot be empty"
        );
    }

    if (agents_.find(agent_name) != agents_.end()) {
        return Result::failure(
            ErrorCode::AgentAlreadyExists,
            "agent already exists: " + agent_name
        );
    }

    agents_[agent_name] = std::move(agent);
    return Result::success();
}

Result AgentRuntime::unregister_agent(const std::string& name) {
    auto it = agents_.find(name);

    if (it == agents_.end()) {
        return Result::failure(
            ErrorCode::AgentNotFound,
            "agent not found: " + name
        );
    }

    if (running_) {
        it->second->on_stop();
    }

    agents_.erase(it);
    return Result::success();
}

Result AgentRuntime::send(const Message& message) {
    if (!transport_.send(message)) {
        return Result::failure(
            ErrorCode::TransportError,
            "failed to send message"
        );
    }

    return Result::success();
}

Result AgentRuntime::publish_event(
    const std::string& source,
    const std::string& target,
    Event event
) {
    Message message = Message::event_message(
        next_message_id(),
        source,
        target,
        std::move(event)
    );

    return send(message);
}

Result AgentRuntime::broadcast_event(
    const std::string& source,
    Event event
) {
    for (const auto& pair : agents_) {
        Message message = Message::event_message(
            next_message_id(),
            source,
            pair.first,
            event
        );

        auto result = send(message);

        if (!result.ok) {
            return result;
        }
    }

    return Result::success();
}

void AgentRuntime::start() {
    if (running_) {
        return;
    }

    running_ = true;

    for (auto& pair : agents_) {
        pair.second->on_start();
    }
}

void AgentRuntime::stop() {
    if (!running_) {
        return;
    }

    for (auto& pair : agents_) {
        pair.second->on_stop();
    }

    running_ = false;
}

Result AgentRuntime::run_once() {
    if (!running_) {
        return Result::failure(
            ErrorCode::RuntimeNotRunning,
            "runtime is not running"
        );
    }

    auto message = transport_.receive();

    if (!message.has_value()) {
        return Result::success();
    }

    return dispatch(*message);
}

void AgentRuntime::drain() {
    while (!transport_.empty()) {
        auto result = run_once();

        if (!result.ok) {
            std::cerr << "[Galculus] runtime error: "
                      << result.message << '\n';
            break;
        }
    }
}

bool AgentRuntime::is_running() const {
    return running_;
}

bool AgentRuntime::has_agent(const std::string& name) const {
    return agents_.find(name) != agents_.end();
}

std::size_t AgentRuntime::agent_count() const {
    return agents_.size();
}

MessageId AgentRuntime::next_message_id() {
    return next_message_id_++;
}

Result AgentRuntime::dispatch(const Message& message) {
    auto it = agents_.find(message.target);

    if (it == agents_.end()) {
        return Result::failure(
            ErrorCode::AgentNotFound,
            "unknown target agent: " + message.target
        );
    }

    it->second->on_message(message);
    return Result::success();
}

} // namespace galculus::core