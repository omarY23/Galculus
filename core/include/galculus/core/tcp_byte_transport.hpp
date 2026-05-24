#pragma once

#include <galculus/core/byte_transport.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace galculus::core {

class TcpByteTransport final : public ByteTransport {
public:
    ~TcpByteTransport() override;

    TcpByteTransport(const TcpByteTransport&) = delete;
    TcpByteTransport& operator=(const TcpByteTransport&) = delete;

    TcpByteTransport(TcpByteTransport&&) = delete;
    TcpByteTransport& operator=(TcpByteTransport&&) = delete;

    static std::unique_ptr<TcpByteTransport> connect_to(
        const std::string& host,
        std::uint16_t port
    );

    static std::unique_ptr<TcpByteTransport> listen_once(
        std::uint16_t port
    );

    bool send_bytes(const std::vector<std::uint8_t>& bytes) override;

    std::optional<std::vector<std::uint8_t>> receive_bytes() override;

    bool empty() const override;

private:
    explicit TcpByteTransport(std::uintptr_t socket_handle);

    std::uintptr_t socket_handle_{0};
};

} // namespace galculus::core