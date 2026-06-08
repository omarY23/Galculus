#include <cstdint>
#include <iostream>

#include "galculus/memory/AgentMemoryBudget.hpp"
#include "galculus/memory/FixedBlockPool.hpp"
#include "galculus/memory/MemoryArena.hpp"
#include "galculus/memory/MemoryManager.hpp"
#include "galculus/memory/ScratchBuffer.hpp"

using namespace galculus::memory;

#define CHECK(expr)                                                       \
    do {                                                                  \
        if (!(expr)) {                                                     \
            std::cerr << "[FAILED] " << __FUNCTION__                      \
                      << " line " << __LINE__                             \
                      << " check: " << #expr << "\n";                     \
            return false;                                                 \
        }                                                                 \
    } while (0)

static bool test_arena_allocation_succeeds() {
    std::uint8_t buffer[128];
    MemoryArena arena(buffer, sizeof(buffer));

    auto block = arena.allocate(32, 8);

    CHECK(block);
    CHECK(block.size == 32);
    CHECK(arena.used() >= 32);
    CHECK(arena.free() <= 96);

    return true;
}

static bool test_arena_allocation_fails_when_exceeded() {
    std::uint8_t buffer[64];
    MemoryArena arena(buffer, sizeof(buffer));

    auto a = arena.allocate(48, 8);
    auto b = arena.allocate(48, 8);

    CHECK(a);
    CHECK(!b);

    return true;
}

static bool test_arena_alignment() {
    std::uint8_t buffer[128];
    MemoryArena arena(buffer, sizeof(buffer));

    auto block = arena.allocate(16, 32);

    CHECK(block);

    auto address =
        reinterpret_cast<std::uintptr_t>(block.ptr);

    CHECK(address % 32 == 0);

    return true;
}

static bool test_arena_reset() {
    std::uint8_t buffer[128];
    MemoryArena arena(buffer, sizeof(buffer));

    auto block = arena.allocate(64, 8);

    CHECK(block);
    CHECK(arena.used() > 0);

    arena.reset();

    CHECK(arena.used() == 0);
    CHECK(arena.free() == 128);

    return true;
}

static bool test_arena_peak_usage() {
    std::uint8_t buffer[128];
    MemoryArena arena(buffer, sizeof(buffer));

    auto a = arena.allocate(32, 8);
    auto b = arena.allocate(16, 8);

    CHECK(a);
    CHECK(b);
    CHECK(arena.peak_used() >= 48);

    arena.reset();

    CHECK(arena.used() == 0);
    CHECK(arena.peak_used() >= 48);

    return true;
}

static bool test_fixed_block_pool_allocates_and_reuses() {
    FixedBlockPool pool(32, 2);

    auto a = pool.allocate();
    auto b = pool.allocate();

    CHECK(a);
    CHECK(b);
    CHECK(pool.used_blocks() == 2);
    CHECK(pool.free_blocks() == 0);

    CHECK(pool.free_block(a.ptr));
    CHECK(pool.used_blocks() == 1);
    CHECK(pool.free_blocks() == 1);

    auto c = pool.allocate();

    CHECK(c);
    CHECK(c.ptr == a.ptr);

    return true;
}

static bool test_fixed_block_pool_fails_when_exhausted() {
    FixedBlockPool pool(32, 1);

    auto a = pool.allocate();
    auto b = pool.allocate();

    CHECK(a);
    CHECK(!b);

    return true;
}

static bool test_fixed_block_pool_rejects_invalid_free() {
    FixedBlockPool pool(32, 1);

    int fake = 0;

    CHECK(!pool.free_block(&fake));

    return true;
}

static bool test_scratch_buffer_reset() {
    std::uint8_t buffer[128];
    MemoryArena arena(buffer, sizeof(buffer));
    ScratchBuffer scratch(arena);

    auto a = scratch.allocate(64, 8);

    CHECK(a);
    CHECK(scratch.stats().used > 0);

    scratch.reset();

    CHECK(scratch.stats().used == 0);

    auto b = scratch.allocate(128, 8);

    CHECK(b);

    return true;
}

static bool test_agent_budget_allows_within_limit() {
    AgentMemoryBudget budget("planner", 128);

    CHECK(budget.reserve(64));
    CHECK(budget.used_bytes() == 64);
    CHECK(budget.reserve(32));
    CHECK(budget.used_bytes() == 96);

    return true;
}

static bool test_agent_budget_rejects_over_limit() {
    AgentMemoryBudget budget("planner", 128);

    CHECK(budget.reserve(100));
    CHECK(!budget.reserve(40));
    CHECK(budget.used_bytes() == 100);

    return true;
}

static bool test_agent_budget_release() {
    AgentMemoryBudget budget("planner", 128);

    CHECK(budget.reserve(100));

    budget.release(30);

    CHECK(budget.used_bytes() == 70);

    budget.release(1000);

    CHECK(budget.used_bytes() == 0);

    return true;
}

static bool test_memory_manager_global_and_scratch() {
    MemoryManagerConfig config;
    config.global_arena_size = 256;
    config.scratch_arena_size = 128;
    config.message_block_size = 64;
    config.message_block_count = 2;

    MemoryManager manager(config);

    auto global = manager.allocate_global(128, 8);
    auto scratch = manager.allocate_scratch(64, 8);

    CHECK(global);
    CHECK(scratch);

    CHECK(manager.global_stats().used >= 128);
    CHECK(manager.scratch_stats().used >= 64);

    manager.reset_scratch();

    CHECK(manager.scratch_stats().used == 0);

    return true;
}

static bool test_memory_manager_message_pool() {
    MemoryManagerConfig config;
    config.global_arena_size = 256;
    config.scratch_arena_size = 128;
    config.message_block_size = 64;
    config.message_block_count = 1;

    MemoryManager manager(config);

    auto msg1 = manager.allocate_message();
    auto msg2 = manager.allocate_message();

    CHECK(msg1);
    CHECK(!msg2);

    CHECK(manager.free_message(msg1.ptr));

    auto msg3 = manager.allocate_message();

    CHECK(msg3);
    CHECK(msg3.ptr == msg1.ptr);

    return true;
}

static bool test_memory_manager_agent_budget() {
    MemoryManagerConfig config;
    MemoryManager manager(config);

    manager.set_agent_budget("planner", 128);

    CHECK(manager.reserve_agent_memory("planner", 64));
    CHECK(!manager.reserve_agent_memory("planner", 100));

    const auto* budget =
        manager.agent_budget("planner");

    CHECK(budget != nullptr);
    CHECK(budget->used_bytes() == 64);

    manager.release_agent_memory("planner", 32);

    CHECK(budget->used_bytes() == 32);

    return true;
}

int main() {
    int failed = 0;

    failed += !test_arena_allocation_succeeds();
    failed += !test_arena_allocation_fails_when_exceeded();
    failed += !test_arena_alignment();
    failed += !test_arena_reset();
    failed += !test_arena_peak_usage();
    failed += !test_fixed_block_pool_allocates_and_reuses();
    failed += !test_fixed_block_pool_fails_when_exhausted();
    failed += !test_fixed_block_pool_rejects_invalid_free();
    failed += !test_scratch_buffer_reset();
    failed += !test_agent_budget_allows_within_limit();
    failed += !test_agent_budget_rejects_over_limit();
    failed += !test_agent_budget_release();
    failed += !test_memory_manager_global_and_scratch();
    failed += !test_memory_manager_message_pool();
    failed += !test_memory_manager_agent_budget();

    if (failed != 0) {
        std::cerr << "[memory_tests] failed tests: "
                  << failed << "\n";
        return 1;
    }

    std::cout << "[memory_tests] all tests passed\n";
    return 0;
}