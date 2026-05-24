#pragma once

#include <galculus/core/message.hpp>
#include <galculus/core/result.hpp>

#include <cstdint>
#include <vector>

namespace galculus::core {

struct DecodeResult {
    Result result;
    Message message;
};

class Protocol {
public:
    static std::vector<std::uint8_t> encode(const Message& message);

    static DecodeResult decode(const std::vector<std::uint8_t>& bytes);

private:
    static constexpr std::uint32_t MAGIC = 0x47434C43; // "GCLC"
    static constexpr std::uint16_t VERSION = 1;
};

} // namespace galculus::core