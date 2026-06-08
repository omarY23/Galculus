#include "galculus/memory/MemoryManager.hpp"

namespace galculus::memory {

MemoryManager::MemoryManager(MemoryManagerConfig config)
    : config_(config),
      global_storage_(config.global_arena_size),
      scratch_storage_(config.scratch_arena_size),
      global_arena_(global_storage_.data(), global_storage_.size()),
      scratch_arena_(scratch_storage_.data(), scratch_storage_.size()),
      scratch_buffer_(scratch_arena_),
      message_pool_(
          config.message_block_size,
          config.message_block_count
      ) {}

MemoryBlock MemoryManager::allocate_global(
    std::size_t size,
    std::size_t alignment
) {
    return global_arena_.allocate(size, alignment);
}

MemoryBlock MemoryManager::allocate_scratch(
    std::size_t size,
    std::size_t alignment
) {
    return scratch_buffer_.allocate(size, alignment);
}

void MemoryManager::reset_scratch() {
    scratch_buffer_.reset();
}

MemoryBlock MemoryManager::allocate_message() {
    return message_pool_.allocate();
}

bool MemoryManager::free_message(void* ptr) {
    return message_pool_.free_block(ptr);
}

void MemoryManager::set_agent_budget(
    const std::string& agent_id,
    std::size_t limit_bytes
) {
    agent_budgets_[agent_id] =
        AgentMemoryBudget(agent_id, limit_bytes);
}

bool MemoryManager::reserve_agent_memory(
    const std::string& agent_id,
    std::size_t bytes
) {
    auto it = agent_budgets_.find(agent_id);

    if (it == agent_budgets_.end()) {
        return false;
    }

    return it->second.reserve(bytes);
}

void MemoryManager::release_agent_memory(
    const std::string& agent_id,
    std::size_t bytes
) {
    auto it = agent_budgets_.find(agent_id);

    if (it == agent_budgets_.end()) {
        return;
    }

    it->second.release(bytes);
}

const AgentMemoryBudget* MemoryManager::agent_budget(
    const std::string& agent_id
) const {
    auto it = agent_budgets_.find(agent_id);

    if (it == agent_budgets_.end()) {
        return nullptr;
    }

    return &it->second;
}

MemoryStats MemoryManager::global_stats() const {
    return global_arena_.stats();
}

MemoryStats MemoryManager::scratch_stats() const {
    return scratch_buffer_.stats();
}

MemoryStats MemoryManager::message_pool_stats() const {
    return message_pool_.stats();
}

}