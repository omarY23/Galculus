#include "galculus/memory/AgentMemoryBudget.hpp"

#include <algorithm>

namespace galculus::memory {

AgentMemoryBudget::AgentMemoryBudget(
    std::string agent_id,
    std::size_t limit_bytes
)
    : agent_id_(std::move(agent_id)),
      limit_bytes_(limit_bytes) {}

bool AgentMemoryBudget::reserve(std::size_t bytes) {
    if (!can_reserve(bytes)) {
        return false;
    }

    used_bytes_ += bytes;
    peak_used_bytes_ =
        std::max(peak_used_bytes_, used_bytes_);

    return true;
}

void AgentMemoryBudget::release(std::size_t bytes) {
    if (bytes >= used_bytes_) {
        used_bytes_ = 0;
        return;
    }

    used_bytes_ -= bytes;
}

bool AgentMemoryBudget::can_reserve(std::size_t bytes) const {
    return used_bytes_ + bytes <= limit_bytes_;
}

MemoryStats AgentMemoryBudget::stats() const {
    return MemoryStats{
        limit_bytes_,
        used_bytes_,
        limit_bytes_ - used_bytes_,
        peak_used_bytes_
    };
}

const std::string& AgentMemoryBudget::agent_id() const {
    return agent_id_;
}

std::size_t AgentMemoryBudget::limit_bytes() const {
    return limit_bytes_;
}

std::size_t AgentMemoryBudget::used_bytes() const {
    return used_bytes_;
}

std::size_t AgentMemoryBudget::peak_used_bytes() const {
    return peak_used_bytes_;
}

}