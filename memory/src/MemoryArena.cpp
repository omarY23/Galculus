#include "galculus/memory/MemoryArena.hpp"

#include <algorithm>
#include <cstdint>

namespace galculus::memory {

MemoryArena::MemoryArena(void* buffer, std::size_t capacity)
    : buffer_(static_cast<std::uint8_t*>(buffer)),
      capacity_(capacity) {}

MemoryBlock MemoryArena::allocate(
    std::size_t size,
    std::size_t alignment
) {
    if (buffer_ == nullptr || size == 0) {
        return {};
    }

    if (alignment == 0 || !is_power_of_two(alignment)) {
        return {};
    }

    const std::uintptr_t base =
        reinterpret_cast<std::uintptr_t>(buffer_);

    const std::uintptr_t current =
        base + offset_;

    const std::uintptr_t aligned =
        (current + alignment - 1) & ~(alignment - 1);

    const std::size_t padding =
        static_cast<std::size_t>(aligned - current);

    const std::size_t new_offset =
        offset_ + padding + size;

    if (new_offset > capacity_) {
        return {};
    }

    offset_ = new_offset;
    peak_used_ = std::max(peak_used_, offset_);

    return MemoryBlock{
        reinterpret_cast<void*>(aligned),
        size
    };
}

void MemoryArena::reset() {
    offset_ = 0;
}

MemoryStats MemoryArena::stats() const {
    return MemoryStats{
        capacity_,
        offset_,
        capacity_ - offset_,
        peak_used_
    };
}

std::size_t MemoryArena::capacity() const {
    return capacity_;
}

std::size_t MemoryArena::used() const {
    return offset_;
}

std::size_t MemoryArena::free() const {
    return capacity_ - offset_;
}

std::size_t MemoryArena::peak_used() const {
    return peak_used_;
}

bool MemoryArena::is_power_of_two(std::size_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

}