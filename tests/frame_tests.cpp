#include <galculus/core/frame.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/protocol.hpp>

#include <cassert>

using namespace galculus::core;

int main() {
    Message original = Message::text_event(
        99,
        "esp32",
        "planner",
        "motion",
        "person detected"
    );

    auto packet = Protocol::encode(original);
    auto frame = Frame::encode(packet);

    assert(!frame.empty());

    auto decoded_frame = Frame::decode(frame);

    assert(decoded_frame.result.ok);
    assert(decoded_frame.payload == packet);

    auto decoded_message = Protocol::decode(decoded_frame.payload);

    assert(decoded_message.result.ok);
    assert(decoded_message.message.id == 99);
    assert(decoded_message.message.source == "esp32");
    assert(decoded_message.message.target == "planner");
    assert(decoded_message.message.event.name == "motion");
    assert(decoded_message.message.event.payload_as_string() == "person detected");

    frame[10] ^= 0xFF;

    auto corrupted = Frame::decode(frame);

    assert(!corrupted.result.ok);
    assert(corrupted.result.code == ErrorCode::InvalidArgument);

    return 0;
}