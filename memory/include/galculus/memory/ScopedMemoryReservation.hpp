#pragma once

#include <cstddef>
#include <string>

#include "galculus/memory/MemoryManager.hpp"

namespace galculus::memory {

class ScopedMemoryReservation {
public:
    ScopedMemoryReservation(
        MemoryManager& manager,
        std::string agent_id,
        std::size_t bytes
    );

    ~ScopedMemoryReservation();

    ScopedMemoryReservation(const ScopedMemoryReservation&) = delete;
    ScopedMemoryReservation& operator=(const ScopedMemoryReservation&) = delete;

    bool ok() const;

private:
    MemoryManager& manager_;
    std::string agent_id_;
    std::size_t bytes_ = 0;
    bool reserved_ = false;
};

}