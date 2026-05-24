#pragma once

#include <galculus/core/message.hpp>

#include <optional>

namespace galculus::core {

class Transport {
public:
    virtual ~Transport() = default;

    virtual bool send(const Message& message) = 0;

    virtual std::optional<Message> receive() = 0;

    virtual bool empty() const = 0;
};

} // namespace galculus::core