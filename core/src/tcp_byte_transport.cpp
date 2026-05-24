#include <galculus/core/tcp_byte_transport.hpp>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #error "TcpByteTransport currently supports Windows only"
#endif

#include <array>
#include <cstring>
#include <iostream>

namespace galculus::core {

namespace {

class WinsockRuntime {
public:
    WinsockRuntime() {
        WSADATA data;
        WSAStartup(MAKEWORD(2, 2), &data);
    }

    ~WinsockRuntime() {
        WSACleanup();
    }
};

WinsockRuntime& winsock_runtime() {
    static WinsockRuntime runtime;
    return runtime;
}

bool send_all(SOCKET socket, const std::uint8_t* data, int size) {
    int total_sent = 0;

    while (total_sent < size) {
        int sent = send(
            socket,
            reinterpret_cast<const char*>(data + total_sent),
            size - total_sent,
            0
        );

        if (sent <= 0) {
            return false;
        }

        total_sent += sent;
    }

    return true;
}

bool recv_all(SOCKET socket, std::uint8_t* data, int size) {
    int total_received = 0;

    while (total_received < size) {
        int received = recv(
            socket,
            reinterpret_cast<char*>(data + total_received),
            size - total_received,
            0
        );

        if (received <= 0) {
            return false;
        }

        total_received += received;
    }

    return true;
}

std::uint32_t read_u32_be(const std::uint8_t* data) {
    return
        (static_cast<std::uint32_t>(data[0]) << 24) |
        (static_cast<std::uint32_t>(data[1]) << 16) |
        (static_cast<std::uint32_t>(data[2]) << 8) |
        static_cast<std::uint32_t>(data[3]);
}

} // namespace

TcpByteTransport::TcpByteTransport(std::uintptr_t socket_handle)
    : socket_handle_(socket_handle) {
    winsock_runtime();
}

TcpByteTransport::~TcpByteTransport() {
    if (socket_handle_ != 0) {
        closesocket(static_cast<SOCKET>(socket_handle_));
        socket_handle_ = 0;
    }
}

std::unique_ptr<TcpByteTransport> TcpByteTransport::connect_to(
    const std::string& host,
    std::uint16_t port
) {
    winsock_runtime();

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (client_socket == INVALID_SOCKET) {
        return nullptr;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    int converted = inet_pton(AF_INET, host.c_str(), &address.sin_addr);

    if (converted != 1) {
        closesocket(client_socket);
        return nullptr;
    }

    int result = connect(
        client_socket,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    );

    if (result == SOCKET_ERROR) {
        closesocket(client_socket);
        return nullptr;
    }

    return std::unique_ptr<TcpByteTransport>(
        new TcpByteTransport(static_cast<std::uintptr_t>(client_socket))
    );
}

std::unique_ptr<TcpByteTransport> TcpByteTransport::listen_once(
    std::uint16_t port
) {
    winsock_runtime();

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listen_socket == INVALID_SOCKET) {
        return nullptr;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int bind_result = bind(
        listen_socket,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    );

    if (bind_result == SOCKET_ERROR) {
        closesocket(listen_socket);
        return nullptr;
    }

    int listen_result = listen(listen_socket, 1);

    if (listen_result == SOCKET_ERROR) {
        closesocket(listen_socket);
        return nullptr;
    }

    SOCKET accepted_socket = accept(listen_socket, nullptr, nullptr);

    closesocket(listen_socket);

    if (accepted_socket == INVALID_SOCKET) {
        return nullptr;
    }

    return std::unique_ptr<TcpByteTransport>(
        new TcpByteTransport(static_cast<std::uintptr_t>(accepted_socket))
    );
}

bool TcpByteTransport::send_bytes(
    const std::vector<std::uint8_t>& bytes
) {
    if (socket_handle_ == 0 || bytes.empty()) {
        return false;
    }

    return send_all(
        static_cast<SOCKET>(socket_handle_),
        bytes.data(),
        static_cast<int>(bytes.size())
    );
}

std::optional<std::vector<std::uint8_t>>
TcpByteTransport::receive_bytes() {
    if (socket_handle_ == 0) {
        return std::nullopt;
    }

    std::array<std::uint8_t, 8> header{};

    if (!recv_all(
        static_cast<SOCKET>(socket_handle_),
        header.data(),
        static_cast<int>(header.size())
    )) {
        return std::nullopt;
    }

    std::uint32_t payload_size = read_u32_be(header.data() + 4);

    const std::uint32_t checksum_size = 4;
    const std::uint32_t remaining_size = payload_size + checksum_size;

    std::vector<std::uint8_t> frame;
    frame.insert(frame.end(), header.begin(), header.end());
    frame.resize(8 + remaining_size);

    if (!recv_all(
        static_cast<SOCKET>(socket_handle_),
        frame.data() + 8,
        static_cast<int>(remaining_size)
    )) {
        return std::nullopt;
    }

    return frame;
}

bool TcpByteTransport::empty() const {
    if (socket_handle_ == 0) {
        return true;
    }

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(static_cast<SOCKET>(socket_handle_), &read_set);

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int result = select(
        0,
        &read_set,
        nullptr,
        nullptr,
        &timeout
    );

    return result <= 0;
}

} // namespace galculus::core