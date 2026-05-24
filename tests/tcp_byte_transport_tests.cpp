#include <galculus/core/framed_message_transport.hpp>
#include <galculus/core/message.hpp>
#include <galculus/core/tcp_byte_transport.hpp>

#include <cassert>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

using namespace galculus::core;

int main() {
    constexpr std::uint16_t port = 39091;

    bool server_received = false;
    std::string server_payload;

    std::thread server_thread([&]() {
        auto server_byte_transport = TcpByteTransport::listen_once(port);

        assert(server_byte_transport);

        FramedMessageTransport server_transport(*server_byte_transport);

        auto received = server_transport.receive();

        assert(received.has_value());

        server_received = true;
        server_payload = received->event.payload_as_string();

        Message reply = Message::text_event(
            2,
            "server",
            "client",
            "reply",
            "ack"
        );

        assert(server_transport.send(reply));
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto client_byte_transport = TcpByteTransport::connect_to(
        "127.0.0.1",
        port
    );

    assert(client_byte_transport);

    FramedMessageTransport client_transport(*client_byte_transport);

    Message message = Message::text_event(
        1,
        "client",
        "server",
        "hello",
        "tcp works"
    );

    assert(client_transport.send(message));

    auto reply = client_transport.receive();

    assert(reply.has_value());
    assert(reply->source == "server");
    assert(reply->target == "client");
    assert(reply->event.name == "reply");
    assert(reply->event.payload_as_string() == "ack");

    server_thread.join();

    assert(server_received);
    assert(server_payload == "tcp works");

    return 0;
}