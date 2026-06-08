#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "galculus/memory/AgentMemoryBudget.hpp"
#include "galculus/memory/FixedBlockPool.hpp"
#include "galculus/memory/MemoryArena.hpp"
#include "galculus/memory/MemoryBlock.hpp"
#include "galculus/memory/ScratchBuffer.hpp"

namespace galculus::memory {

struct MemoryManagerConfig {
    std::size_t global_arena_size = 64 * 1024;
    std::size_t scratch_arena_size = 16 * 1024;
    std::size_t message_block_size = 512;
    std::size_t message_block_count = 32;
};

class MemoryManager {
public:
    explicit MemoryManager(MemoryManagerConfig config);

    MemoryBlock allocate_global(
        std::size_t size,
        std::size_t alignment = alignof(std::max_align_t)
    );

    MemoryBlock allocate_scratch(
        std::size_t size,
        std::size_t alignment = alignof(std::max_align_t)
    );

    void reset_scratch();

    MemoryBlock allocate_message();

    bool free_message(void* ptr);

    void set_agent_budget(
        const std::string& agent_id,
        std::size_t limit_bytes
    );

    bool reserve_agent_memory(
        const std::string& agent_id,
        std::size_t bytes
    );

    void release_agent_memory(
        const std::string& agent_id,
        std::size_t bytes
    );

    const AgentMemoryBudget* agent_budget(
        const std::string& agent_id
    ) const;

    MemoryStats global_stats() const;
    MemoryStats scratch_stats() const;
    MemoryStats message_pool_stats() const;

private:
    MemoryManagerConfig config_;

    std::vector<std::uint8_t> global_storage_;
    std::vector<std::uint8_t> scratch_storage_;

    MemoryArena global_arena_;
    MemoryArena scratch_arena_;
    ScratchBuffer scratch_buffer_;
    FixedBlockPool message_pool_;

    std::unordered_map<std::string, AgentMemoryBudget> agent_budgets_;
};

}