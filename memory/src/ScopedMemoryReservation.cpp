#include "galculus/memory/ScopedMemoryReservation.hpp"

namespace galculus::memory {

ScopedMemoryReservation::ScopedMemoryReservation(
    MemoryManager& manager,
    std::string agent_id,
    std::size_t bytes
)
    : manager_(manager),
      agent_id_(std::move(agent_id)),
      bytes_(bytes) {
    reserved_ =
        manager_.reserve_agent_memory(
            agent_id_,
            bytes_
        );
}

ScopedMemoryReservation::~ScopedMemoryReservation() {
    if (reserved_) {
        manager_.release_agent_memory(
            agent_id_,
            bytes_
        );
    }
}

bool ScopedMemoryReservation::ok() const {
    return reserved_;
}

}