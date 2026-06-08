#include "galculus/memory/ScratchBuffer.hpp"

namespace galculus::memory {

ScratchBuffer::ScratchBuffer(MemoryArena& arena)
    : arena_(arena) {}

MemoryBlock ScratchBuffer::allocate(
    std::size_t size,
    std::size_t alignment
) {
    return arena_.allocate(size, alignment);
}

void ScratchBuffer::reset() {
    arena_.reset();
}

MemoryStats ScratchBuffer::stats() const {
    return arena_.stats();
}

}