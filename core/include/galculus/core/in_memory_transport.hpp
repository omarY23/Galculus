#pragma once

#include <galculus/core/transport.hpp>

#include <mutex>
#include <queue>

namespace galculus::core {

class InMemoryTransport final : public Transport {
public:
    bool send(const Message& message) override;

    std::optional<Message> receive() override;

    bool empty() const override;

private:
    mutable std::mutex mutex_;
    std::queue<Message> queue_;
};

} // namespace galculus::core