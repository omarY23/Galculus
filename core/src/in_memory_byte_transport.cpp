#include <galculus/core/in_memory_byte_transport.hpp>

namespace galculus::core {

bool InMemoryByteTransport::send_bytes(
    const std::vector<std::uint8_t>& bytes
) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(bytes);
    return true;
}

std::optional<std::vector<std::uint8_t>>
InMemoryByteTransport::receive_bytes() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }

    auto bytes = queue_.front();
    queue_.pop();

    return bytes;
}

bool InMemoryByteTransport::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

} // namespace galculus::core