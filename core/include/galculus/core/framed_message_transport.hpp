#pragma once

#include <galculus/core/message_byte_adapter.hpp>
#include <galculus/core/transport.hpp>

namespace galculus::core {

class FramedMessageTransport final : public Transport {
public:
    explicit FramedMessageTransport(ByteTransport& byte_transport);

    bool send(const Message& message) override;

    std::optional<Message> receive() override;

    bool empty() const override;

private:
    MessageByteAdapter adapter_;
};

} // namespace galculus::core