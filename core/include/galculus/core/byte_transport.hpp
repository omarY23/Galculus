#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace galculus::core {

class ByteTransport {
public:
    virtual ~ByteTransport() = default;

    virtual bool send_bytes(const std::vector<std::uint8_t>& bytes) = 0;

    virtual std::optional<std::vector<std::uint8_t>> receive_bytes() = 0;

    virtual bool empty() const = 0;
};

} // namespace galculus::core