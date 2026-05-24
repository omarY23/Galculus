#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <cstdint>
#include <iostream>

using namespace galculus::core;

int main() {
    constexpr std::uint16_t port = 39100;

    std::cout << "[client] connecting to 127.0.0.1:" << port << "\n";

    auto byte_transport = TcpByteTransport::connect_to("127.0.0.1", port);

    if (!byte_transport) {
        std::cerr << "[client] failed to connect\n";
        return 1;
    }

    FramedMessageTransport transport(*byte_transport);

    Message message = Message::text_event(
        1,
        "client.sensor",
        "server.planner",
        "motion_detected",
        "person at door"
    );

    if (!transport.send(message)) {
        std::cerr << "[client] failed to send message\n";
        return 1;
    }

    std::cout << "[client] message sent\n";

    auto reply = transport.receive();

    if (!reply.has_value()) {
        std::cerr << "[client] failed to receive reply\n";
        return 1;
    }

    std::cout << "[client] received reply\n";
    std::cout << "  event: " << reply->event.name << "\n";
    std::cout << "  payload: " << reply->event.payload_as_string() << "\n";

    return 0;
}