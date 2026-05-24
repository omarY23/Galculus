#include <galculus/core/in_memory_byte_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/message_byte_adapter.hpp>

#include <cassert>

using namespace galculus::core;

int main() {
    InMemoryByteTransport byte_transport;
    MessageByteAdapter adapter(byte_transport);

    Message original = Message::text_event(
        123,
        "camera",
        "planner",
        "object_detected",
        "package"
    );

    auto send_result = adapter.send_message(original);

    assert(send_result.ok);
    assert(!adapter.empty());

    auto receive_result = adapter.receive_message();

    assert(receive_result.result.ok);
    assert(receive_result.message.has_value());

    Message received = *receive_result.message;

    assert(received.id == 123);
    assert(received.type == MessageType::Event);
    assert(received.source == "camera");
    assert(received.target == "planner");
    assert(received.event.name == "object_detected");
    assert(received.event.payload_as_string() == "package");

    assert(adapter.empty());

    auto empty_receive = adapter.receive_message();

    assert(empty_receive.result.ok);
    assert(!empty_receive.message.has_value());

    return 0;
}