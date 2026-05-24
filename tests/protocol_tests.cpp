#include <galculus/core/message.hpp>
#include <galculus/core/protocol.hpp>

#include <cassert>
#include <cstdint>
#include <vector>

using namespace galculus::core;

int main() {
    Message original = Message::text_event(
        42,
        "sensor",
        "planner",
        "motion_detected",
        "person at door"
    );

    std::vector<std::uint8_t> encoded = Protocol::encode(original);

    assert(!encoded.empty());

    DecodeResult decoded = Protocol::decode(encoded);

    assert(decoded.result.ok);

    assert(decoded.message.id == 42);
    assert(decoded.message.type == MessageType::Event);
    assert(decoded.message.source == "sensor");
    assert(decoded.message.target == "planner");
    assert(decoded.message.event.name == "motion_detected");
    assert(decoded.message.event.payload_as_string() == "person at door");

    std::vector<std::uint8_t> invalid = {0x00, 0x01, 0x02};

    DecodeResult bad = Protocol::decode(invalid);

    assert(!bad.result.ok);
    assert(bad.result.code == ErrorCode::InvalidArgument);

    return 0;
}