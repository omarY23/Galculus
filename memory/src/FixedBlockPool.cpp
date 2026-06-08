#include "galculus/memory/FixedBlockPool.hpp"

#include <algorithm>
#include <cstdint>

namespace galculus::memory {

FixedBlockPool::FixedBlockPool(
    std::size_t block_size,
    std::size_t block_count
)
    : block_size_(block_size),
      block_count_(block_count),
      storage_(block_size * block_count),
      allocated_(block_count, false) {
    free_indices_.reserve(block_count_);

    for (std::size_t i = 0; i < block_count_; ++i) {
        free_indices_.push_back(block_count_ - 1 - i);
    }
}

MemoryBlock FixedBlockPool::allocate() {
    if (block_size_ == 0 || block_count_ == 0 || free_indices_.empty()) {
        return {};
    }

    const std::size_t index = free_indices_.back();
    free_indices_.pop_back();

    allocated_[index] = true;
    ++used_blocks_;

    peak_used_blocks_ =
        std::max(peak_used_blocks_, used_blocks_);

    return MemoryBlock{
        storage_.data() + index * block_size_,
        block_size_
    };
}

bool FixedBlockPool::free_block(void* ptr) {
    if (!owns_pointer(ptr)) {
        return false;
    }

    auto* byte_ptr = static_cast<std::uint8_t*>(ptr);
    const std::size_t offset =
        static_cast<std::size_t>(byte_ptr - storage_.data());

    if (offset % block_size_ != 0) {
        return false;
    }

    const std::size_t index = offset / block_size_;

    if (index >= block_count_ || !allocated_[index]) {
        return false;
    }

    allocated_[index] = false;
    free_indices_.push_back(index);
    --used_blocks_;

    return true;
}

MemoryStats FixedBlockPool::stats() const {
    const std::size_t capacity_bytes =
        block_size_ * block_count_;

    const std::size_t used_bytes =
        block_size_ * used_blocks_;

    const std::size_t peak_bytes =
        block_size_ * peak_used_blocks_;

    return MemoryStats{
        capacity_bytes,
        used_bytes,
        capacity_bytes - used_bytes,
        peak_bytes
    };
}

std::size_t FixedBlockPool::block_size() const {
    return block_size_;
}

std::size_t FixedBlockPool::block_count() const {
    return block_count_;
}

std::size_t FixedBlockPool::used_blocks() const {
    return used_blocks_;
}

std::size_t FixedBlockPool::free_blocks() const {
    return block_count_ - used_blocks_;
}

bool FixedBlockPool::owns_pointer(void* ptr) const {
    if (ptr == nullptr || storage_.empty()) {
        return false;
    }

    auto* byte_ptr = static_cast<std::uint8_t*>(ptr);

    return byte_ptr >= storage_.data() &&
           byte_ptr < storage_.data() + storage_.size();
}

}