#pragma once

#include <galculus/core/message.hpp>

#include <string>

namespace galculus::core {

class Agent {
public:
    virtual ~Agent() = default;

    virtual std::string name() const = 0;

    virtual void on_start() {}

    virtual void on_message(const Message& message) = 0;

    virtual void on_stop() {}
};

} // namespace galculus::core