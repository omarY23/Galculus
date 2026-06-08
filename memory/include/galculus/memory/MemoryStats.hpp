#pragma once

#include <cstddef>

namespace galculus::memory {

struct MemoryStats {
    std::size_t capacity = 0;
    std::size_t used = 0;
    std::size_t free = 0;
    std::size_t peak_used = 0;
};

}