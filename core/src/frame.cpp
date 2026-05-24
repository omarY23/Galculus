#include <galculus/core/frame.hpp>

namespace galculus::core {

namespace {

void write_u32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

bool read_u32(
    const std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::uint32_t& value
) {
    if (offset + 4 > bytes.size()) {
        return false;
    }

    value =
        (static_cast<std::uint32_t>(bytes[offset]) << 24) |
        (static_cast<std::uint32_t>(bytes[offset + 1]) << 16) |
        (static_cast<std::uint32_t>(bytes[offset + 2]) << 8) |
        static_cast<std::uint32_t>(bytes[offset + 3]);

    return true;
}

std::uint32_t checksum(const std::vector<std::uint8_t>& bytes) {
    std::uint32_t sum = 0;

    for (std::uint8_t byte : bytes) {
        sum += byte;
    }

    return sum;
}

} // namespace

std::vector<std::uint8_t> Frame::encode(
    const std::vector<std::uint8_t>& payload
) {
    std::vector<std::uint8_t> out;

    write_u32(out, MAGIC);
    write_u32(out, static_cast<std::uint32_t>(payload.size()));

    out.insert(out.end(), payload.begin(), payload.end());

    write_u32(out, checksum(payload));

    return out;
}

FrameDecodeResult Frame::decode(
    const std::vector<std::uint8_t>& frame
) {
    FrameDecodeResult decoded;

    std::uint32_t magic = 0;
    std::uint32_t size = 0;
    std::uint32_t expected_checksum = 0;

    if (!read_u32(frame, 0, magic)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: missing magic"
        );
        return decoded;
    }

    if (magic != MAGIC) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: invalid magic"
        );
        return decoded;
    }

    if (!read_u32(frame, 4, size)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: missing size"
        );
        return decoded;
    }

    const std::size_t payload_start = 8;
    const std::size_t checksum_start = payload_start + size;

    if (checksum_start + 4 > frame.size()) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: incomplete frame"
        );
        return decoded;
    }

    decoded.payload.assign(
        frame.begin() + payload_start,
        frame.begin() + checksum_start
    );

    if (!read_u32(frame, checksum_start, expected_checksum)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: missing checksum"
        );
        return decoded;
    }

    if (checksum(decoded.payload) != expected_checksum) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "frame decode failed: checksum mismatch"
        );
        return decoded;
    }

    decoded.result = Result::success();
    return decoded;
}

} // namespace galculus::core