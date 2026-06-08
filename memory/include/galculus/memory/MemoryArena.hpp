#pragma once

#include <cstddef>
#include <cstdint>

#include "galculus/memory/MemoryBlock.hpp"
#include "galculus/memory/MemoryStats.hpp"

namespace galculus::memory {

class MemoryArena {
public:
    MemoryArena(void* buffer, std::size_t capacity);

    MemoryBlock allocate(
        std::size_t size,
        std::size_t alignment = alignof(std::max_align_t)
    );

    void reset();

    MemoryStats stats() const;

    std::size_t capacity() const;
    std::size_t used() const;
    std::size_t free() const;
    std::size_t peak_used() const;

private:
    static bool is_power_of_two(std::size_t value);

private:
    std::uint8_t* buffer_ = nullptr;
    std::size_t capacity_ = 0;
    std::size_t offset_ = 0;
    std::size_t peak_used_ = 0;
};

}