#pragma once

#include <cstddef>
#include <string>

#include "galculus/memory/MemoryStats.hpp"

namespace galculus::memory {

class AgentMemoryBudget {
public:
    AgentMemoryBudget() = default;

    AgentMemoryBudget(
        std::string agent_id,
        std::size_t limit_bytes
    );

    bool reserve(std::size_t bytes);

    void release(std::size_t bytes);

    bool can_reserve(std::size_t bytes) const;

    MemoryStats stats() const;

    const std::string& agent_id() const;
    std::size_t limit_bytes() const;
    std::size_t used_bytes() const;
    std::size_t peak_used_bytes() const;

private:
    std::string agent_id_;
    std::size_t limit_bytes_ = 0;
    std::size_t used_bytes_ = 0;
    std::size_t peak_used_bytes_ = 0;
};

}