#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <cstdint>
#include <iostream>

using namespace galculus::core;

int main() {
    constexpr std::uint16_t port = 39100;

    std::cout << "[server] listening on port " << port << "\n";

    auto byte_transport = TcpByteTransport::listen_once(port);

    if (!byte_transport) {
        std::cerr << "[server] failed to listen\n";
        return 1;
    }

    std::cout << "[server] client connected\n";

    FramedMessageTransport transport(*byte_transport);

    auto received = transport.receive();

    if (!received.has_value()) {
        std::cerr << "[server] failed to receive message\n";
        return 1;
    }

    std::cout << "[server] received message\n";
    std::cout << "  id: " << received->id << "\n";
    std::cout << "  source: " << received->source << "\n";
    std::cout << "  target: " << received->target << "\n";
    std::cout << "  event: " << received->event.name << "\n";
    std::cout << "  payload: " << received->event.payload_as_string() << "\n";

    Message reply = Message::text_event(
        2,
        "server",
        received->source,
        "ack",
        "message received"
    );

    if (!transport.send(reply)) {
        std::cerr << "[server] failed to send reply\n";
        return 1;
    }

    std::cout << "[server] reply sent\n";

    return 0;
}