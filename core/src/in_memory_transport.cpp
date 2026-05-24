#include <galculus/core/in_memory_transport.hpp>

namespace galculus::core {

bool InMemoryTransport::send(const Message& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(message);
    return true;
}

std::optional<Message> InMemoryTransport::receive() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }

    Message message = queue_.front();
    queue_.pop();

    return message;
}

bool InMemoryTransport::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

} // namespace galculus::core