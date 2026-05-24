#pragma once

#include <galculus/core/result.hpp>

#include <cstdint>
#include <vector>

namespace galculus::core {

struct FrameDecodeResult {
    Result result;
    std::vector<std::uint8_t> payload;
};

class Frame {
public:
    static std::vector<std::uint8_t> encode(
        const std::vector<std::uint8_t>& payload
    );

    static FrameDecodeResult decode(
        const std::vector<std::uint8_t>& frame
    );

private:
    static constexpr std::uint32_t MAGIC = 0x47464D45; // "GFME"
};

} // namespace galculus::core