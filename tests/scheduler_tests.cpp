#include <iostream>
#include <string>

#include "galculus/scheduler/TokenScheduler.hpp"

using namespace galculus::scheduler;

#define CHECK(expr)                                                       \
    do {                                                                  \
        if (!(expr)) {                                                     \
            std::cerr << "[FAILED] " << __FUNCTION__                      \
                      << " line " << __LINE__                             \
                      << " check: " << #expr << "\n";                     \
            return false;                                                 \
        }                                                                 \
    } while (0)

static TokenBudget make_budget(
    std::uint32_t max_prompt,
    std::uint32_t max_completion,
    std::uint32_t remaining
) {
    TokenBudget budget;
    budget.max_prompt_tokens = max_prompt;
    budget.max_completion_tokens = max_completion;
    budget.remaining_tokens = remaining;
    return budget;
}

static bool test_allows_task_within_budget() {
    TokenScheduler scheduler(make_budget(128, 64, 192));

    ScheduledLLMTask task;
    task.task_id = "task_1";
    task.estimated_prompt_tokens = 40;
    task.requested_completion_tokens = 20;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::Allow);
    CHECK(decision.allowed_completion_tokens == 20);
    CHECK(scheduler.budget().remaining_tokens == 132);

    return true;
}

static bool test_reduces_when_completion_exceeds_max() {
    TokenScheduler scheduler(make_budget(128, 32, 160));

    ScheduledLLMTask task;
    task.task_id = "task_2";
    task.estimated_prompt_tokens = 40;
    task.requested_completion_tokens = 80;
    task.allow_truncation = true;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::ReduceTokens);
    CHECK(decision.allowed_completion_tokens == 32);
    CHECK(scheduler.budget().remaining_tokens == 88);

    return true;
}

static bool test_rejects_prompt_too_large_without_fallback() {
    TokenScheduler scheduler(make_budget(64, 32, 96));

    ScheduledLLMTask task;
    task.task_id = "task_3";
    task.estimated_prompt_tokens = 100;
    task.requested_completion_tokens = 16;
    task.allow_fallback = false;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::Reject);
    CHECK(decision.allowed_completion_tokens == 0);

    return true;
}

static bool test_fallback_prompt_too_large_with_fallback() {
    TokenScheduler scheduler(make_budget(64, 32, 96));

    ScheduledLLMTask task;
    task.task_id = "task_4";
    task.estimated_prompt_tokens = 100;
    task.requested_completion_tokens = 16;
    task.allow_fallback = true;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::UseFallback);
    CHECK(decision.allowed_completion_tokens == 0);

    return true;
}

static bool test_reduces_when_remaining_budget_low() {
    TokenScheduler scheduler(make_budget(128, 64, 50));

    ScheduledLLMTask task;
    task.task_id = "task_5";
    task.estimated_prompt_tokens = 40;
    task.requested_completion_tokens = 30;
    task.allow_truncation = true;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::ReduceTokens);
    CHECK(decision.allowed_completion_tokens == 10);
    CHECK(scheduler.budget().remaining_tokens == 0);

    return true;
}

static bool test_rejects_when_truncation_and_fallback_disabled() {
    TokenScheduler scheduler(make_budget(128, 64, 50));

    ScheduledLLMTask task;
    task.task_id = "task_6";
    task.estimated_prompt_tokens = 40;
    task.requested_completion_tokens = 30;
    task.allow_truncation = false;
    task.allow_fallback = false;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::Reject);
    CHECK(decision.allowed_completion_tokens == 0);

    return true;
}

static bool test_release_restores_budget() {
    TokenScheduler scheduler(make_budget(128, 64, 192));

    ScheduledLLMTask task;
    task.task_id = "task_7";
    task.estimated_prompt_tokens = 40;
    task.requested_completion_tokens = 20;

    auto decision = scheduler.schedule(task);

    CHECK(decision.type == ScheduleDecisionType::Allow);
    CHECK(scheduler.budget().remaining_tokens == 132);

    scheduler.release(60);

    CHECK(scheduler.budget().remaining_tokens == 192);

    return true;
}

int main() {
    int failed = 0;

    failed += !test_allows_task_within_budget();
    failed += !test_reduces_when_completion_exceeds_max();
    failed += !test_rejects_prompt_too_large_without_fallback();
    failed += !test_fallback_prompt_too_large_with_fallback();
    failed += !test_reduces_when_remaining_budget_low();
    failed += !test_rejects_when_truncation_and_fallback_disabled();
    failed += !test_release_restores_budget();

    if (failed != 0) {
        std::cerr << "[scheduler_tests] failed tests: " << failed << "\n";
        return 1;
    }

    std::cout << "[scheduler_tests] all tests passed\n";
    return 0;
}