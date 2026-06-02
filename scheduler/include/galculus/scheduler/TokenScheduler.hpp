#pragma once

#include "galculus/scheduler/TokenBudget.hpp"
#include "galculus/scheduler/ScheduledLLMTask.hpp"
#include "galculus/scheduler/ScheduleDecision.hpp"

namespace galculus::scheduler {

class TokenScheduler {
public:
    explicit TokenScheduler(TokenBudget budget);

    ScheduleDecision schedule(const ScheduledLLMTask& task);

    void release(std::uint32_t used_tokens);

    TokenBudget budget() const;

private:
    TokenBudget budget_;
};

}