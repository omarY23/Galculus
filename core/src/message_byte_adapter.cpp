#include <galculus/core/message_byte_adapter.hpp>

#include <galculus/core/frame.hpp>
#include <galculus/core/protocol.hpp>

namespace galculus::core {

MessageByteAdapter::MessageByteAdapter(ByteTransport& transport)
    : transport_(transport) {}

Result MessageByteAdapter::send_message(const Message& message) {
    auto packet = Protocol::encode(message);
    auto frame = Frame::encode(packet);

    if (!transport_.send_bytes(frame)) {
        return Result::failure(
            ErrorCode::TransportError,
            "failed to send message frame"
        );
    }

    return Result::success();
}

MessageReceiveResult MessageByteAdapter::receive_message() {
    MessageReceiveResult output;

    auto frame_bytes = transport_.receive_bytes();

    if (!frame_bytes.has_value()) {
        output.result = Result::success();
        output.message = std::nullopt;
        return output;
    }

    auto decoded_frame = Frame::decode(*frame_bytes);

    if (!decoded_frame.result.ok) {
        output.result = decoded_frame.result;
        output.message = std::nullopt;
        return output;
    }

    auto decoded_message = Protocol::decode(decoded_frame.payload);

    if (!decoded_message.result.ok) {
        output.result = decoded_message.result;
        output.message = std::nullopt;
        return output;
    }

    output.result = Result::success();
    output.message = decoded_message.message;
    return output;
}

bool MessageByteAdapter::empty() const {
    return transport_.empty();
}

} // namespace galculus::core