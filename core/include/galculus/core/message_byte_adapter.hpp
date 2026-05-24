#pragma once

#include <galculus/core/byte_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/result.hpp>

#include <memory>
#include <optional>

namespace galculus::core {

struct MessageReceiveResult {
    Result result;
    std::optional<Message> message;
};

class MessageByteAdapter {
public:
    explicit MessageByteAdapter(ByteTransport& transport);

    Result send_message(const Message& message);

    MessageReceiveResult receive_message();

    bool empty() const;

private:
    ByteTransport& transport_;
};

} // namespace galculus::core