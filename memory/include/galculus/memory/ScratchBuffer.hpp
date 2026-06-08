#pragma once

#include <cstddef>

#include "galculus/memory/MemoryArena.hpp"
#include "galculus/memory/MemoryBlock.hpp"
#include "galculus/memory/MemoryStats.hpp"

namespace galculus::memory {

class ScratchBuffer {
public:
    explicit ScratchBuffer(MemoryArena& arena);

    MemoryBlock allocate(
        std::size_t size,
        std::size_t alignment = alignof(std::max_align_t)
    );

    void reset();

    MemoryStats stats() const;

private:
    MemoryArena& arena_;
};

}