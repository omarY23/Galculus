#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "galculus/memory/MemoryBlock.hpp"
#include "galculus/memory/MemoryStats.hpp"

namespace galculus::memory {

class FixedBlockPool {
public:
    FixedBlockPool(
        std::size_t block_size,
        std::size_t block_count
    );

    MemoryBlock allocate();

    bool free_block(void* ptr);

    MemoryStats stats() const;

    std::size_t block_size() const;
    std::size_t block_count() const;
    std::size_t used_blocks() const;
    std::size_t free_blocks() const;

private:
    bool owns_pointer(void* ptr) const;

private:
    std::size_t block_size_ = 0;
    std::size_t block_count_ = 0;

    std::vector<std::uint8_t> storage_;
    std::vector<std::size_t> free_indices_;
    std::vector<bool> allocated_;

    std::size_t used_blocks_ = 0;
    std::size_t peak_used_blocks_ = 0;
};

}