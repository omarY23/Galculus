#include <galculus/core/framed_message_transport.hpp>

namespace galculus::core {

FramedMessageTransport::FramedMessageTransport(ByteTransport& byte_transport)
    : adapter_(byte_transport) {}

bool FramedMessageTransport::send(const Message& message) {
    return adapter_.send_message(message).ok;
}

std::optional<Message> FramedMessageTransport::receive() {
    auto result = adapter_.receive_message();

    if (!result.result.ok) {
        return std::nullopt;
    }

    return result.message;
}

bool FramedMessageTransport::empty() const {
    return adapter_.empty();
}

} // namespace galculus::core