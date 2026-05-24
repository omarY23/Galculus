#include <galculus/core/in_memory_byte_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/protocol.hpp>

#include <cassert>

using namespace galculus::core;

int main() {
    InMemoryByteTransport transport;

    Message original = Message::text_event(
        7,
        "esp32",
        "planner",
        "wake_word",
        "hello galculus"
    );

    auto encoded = Protocol::encode(original);

    assert(!encoded.empty());
    assert(transport.empty());

    bool sent = transport.send_bytes(encoded);

    assert(sent);
    assert(!transport.empty());

    auto received_bytes = transport.receive_bytes();

    assert(received_bytes.has_value());
    assert(transport.empty());

    auto decoded = Protocol::decode(*received_bytes);

    assert(decoded.result.ok);
    assert(decoded.message.id == 7);
    assert(decoded.message.source == "esp32");
    assert(decoded.message.target == "planner");
    assert(decoded.message.event.name == "wake_word");
    assert(decoded.message.event.payload_as_string() == "hello galculus");

    return 0;
}