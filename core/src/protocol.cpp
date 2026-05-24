#include <galculus/core/protocol.hpp>

#include <cstring>
#include <string>

namespace galculus::core {

namespace {

void write_u16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void write_u32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void write_u64(std::vector<std::uint8_t>& out, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        out.push_back(static_cast<std::uint8_t>((value >> shift) & 0xFF));
    }
}

void write_string(std::vector<std::uint8_t>& out, const std::string& value) {
    write_u32(out, static_cast<std::uint32_t>(value.size()));
    out.insert(out.end(), value.begin(), value.end());
}

void write_bytes(
    std::vector<std::uint8_t>& out,
    const std::vector<std::uint8_t>& value
) {
    write_u32(out, static_cast<std::uint32_t>(value.size()));
    out.insert(out.end(), value.begin(), value.end());
}

bool read_u16(
    const std::vector<std::uint8_t>& bytes,
    std::size_t& offset,
    std::uint16_t& value
) {
    if (offset + 2 > bytes.size()) {
        return false;
    }

    value = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(bytes[offset]) << 8) |
        static_cast<std::uint16_t>(bytes[offset + 1])
    );

    offset += 2;
    return true;
}

bool read_u32(
    const std::vector<std::uint8_t>& bytes,
    std::size_t& offset,
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

    offset += 4;
    return true;
}

bool read_u64(
    const std::vector<std::uint8_t>& bytes,
    std::size_t& offset,
    std::uint64_t& value
) {
    if (offset + 8 > bytes.size()) {
        return false;
    }

    value = 0;

    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | static_cast<std::uint64_t>(bytes[offset + i]);
    }

    offset += 8;
    return true;
}

bool read_string(
    const std::vector<std::uint8_t>& bytes,
    std::size_t& offset,
    std::string& value
) {
    std::uint32_t size = 0;

    if (!read_u32(bytes, offset, size)) {
        return false;
    }

    if (offset + size > bytes.size()) {
        return false;
    }

    value.assign(
        reinterpret_cast<const char*>(bytes.data() + offset),
        size
    );

    offset += size;
    return true;
}

bool read_bytes(
    const std::vector<std::uint8_t>& bytes,
    std::size_t& offset,
    std::vector<std::uint8_t>& value
) {
    std::uint32_t size = 0;

    if (!read_u32(bytes, offset, size)) {
        return false;
    }

    if (offset + size > bytes.size()) {
        return false;
    }

    value.assign(bytes.begin() + offset, bytes.begin() + offset + size);
    offset += size;

    return true;
}

} // namespace

std::vector<std::uint8_t> Protocol::encode(const Message& message) {
    std::vector<std::uint8_t> out;

    write_u32(out, MAGIC);
    write_u16(out, VERSION);

    out.push_back(static_cast<std::uint8_t>(message.type));

    write_u64(out, message.id);

    write_string(out, message.source);
    write_string(out, message.target);
    write_string(out, message.event.name);
    write_bytes(out, message.event.payload);

    return out;
}

DecodeResult Protocol::decode(const std::vector<std::uint8_t>& bytes) {
    DecodeResult decoded;

    std::size_t offset = 0;

    std::uint32_t magic = 0;
    std::uint16_t version = 0;

    if (!read_u32(bytes, offset, magic)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: missing magic"
        );
        return decoded;
    }

    if (magic != MAGIC) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: invalid magic"
        );
        return decoded;
    }

    if (!read_u16(bytes, offset, version)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: missing version"
        );
        return decoded;
    }

    if (version != VERSION) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: unsupported version"
        );
        return decoded;
    }

    if (offset + 1 > bytes.size()) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: missing message type"
        );
        return decoded;
    }

    decoded.message.type = static_cast<MessageType>(bytes[offset]);
    offset += 1;

    if (!read_u64(bytes, offset, decoded.message.id)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: missing message id"
        );
        return decoded;
    }

    if (!read_string(bytes, offset, decoded.message.source)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: invalid source"
        );
        return decoded;
    }

    if (!read_string(bytes, offset, decoded.message.target)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: invalid target"
        );
        return decoded;
    }

    if (!read_string(bytes, offset, decoded.message.event.name)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: invalid event name"
        );
        return decoded;
    }

    if (!read_bytes(bytes, offset, decoded.message.event.payload)) {
        decoded.result = Result::failure(
            ErrorCode::InvalidArgument,
            "protocol decode failed: invalid payload"
        );
        return decoded;
    }

    decoded.result = Result::success();
    return decoded;
}

} // namespace galculus::core