#pragma once

#include <galculus/core/byte_transport.hpp>

#include <mutex>
#include <queue>

namespace galculus::core {

class InMemoryByteTransport final : public ByteTransport {
public:
    bool send_bytes(const std::vector<std::uint8_t>& bytes) override;

    std::optional<std::vector<std::uint8_t>> receive_bytes() override;

    bool empty() const override;

private:
    mutable std::mutex mutex_;
    std::queue<std::vector<std::uint8_t>> queue_;
};

} // namespace galculus::core