#pragma once

#include <cstddef>

namespace galculus::memory {

struct MemoryBlock {
    void* ptr = nullptr;
    std::size_t size = 0;

    explicit operator bool() const {
        return ptr != nullptr && size > 0;
    }
};

}