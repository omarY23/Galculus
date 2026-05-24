#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/in_memory_byte_transport.hpp>
#include <galculus/core/message.hpp>

#include <cassert>

using namespace galculus::core;

int main() {
    InMemoryByteTransport byte_transport;
    FramedMessageTransport transport(byte_transport);

    Message original = Message::text_event(
        55,
        "sensor",
        "planner",
        "temperature",
        "72F"
    );

    bool sent = transport.send(original);

    assert(sent);
    assert(!transport.empty());

    auto received = transport.receive();

    assert(received.has_value());
    assert(received->id == 55);
    assert(received->source == "sensor");
    assert(received->target == "planner");
    assert(received->event.name == "temperature");
    assert(received->event.payload_as_string() == "72F");

    assert(transport.empty());

    return 0;
}