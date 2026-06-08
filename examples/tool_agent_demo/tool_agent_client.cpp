#include <galculus/llm/Prompt.hpp>
#include <galculus/llm/GenerationConfig.hpp>
#include <galculus/llm/LLMResult.hpp>
#include <galculus/llm_remote/RemoteLLM.hpp>

#include <galculus/tools/ToolCallParser.hpp>
#include <galculus/tools/ToolRegistry.hpp>
#include <galculus/tools/SafetyPolicy.hpp>

#include <galculus/scheduler/TokenBudget.hpp>
#include <galculus/scheduler/TokenScheduler.hpp>
#include <galculus/llm_scheduled/ScheduledLLMRuntime.hpp>

#include <galculus/memory/MemoryManager.hpp>
#include <galculus/llm_memory/MemoryAwareLLMRuntime.hpp>

#include <cstdint>
#include <iostream>

int main() {
    using namespace galculus::llm;
    using namespace galculus::llm_remote;
    using namespace galculus::tools;

    constexpr std::uint16_t port = 39200;

    Prompt prompt;
    prompt.add_system(
        "You are a safe embedded device planner. "
        "Respond only using this format:\n"
        "tool=<tool_name>\n"
        "reason=<short_reason>\n"
        "confidence=<0_to_1>\n"
    );

    prompt.add_user(
        "Motion detected near the front door. "
        "Door is locked. It is night. Choose the safest allowed action."
    );

    GenerationConfig config;
    config.max_tokens = 64;
    config.temperature = 0.1f;
    config.top_p = 0.9f;
    config.timeout_ms = 1000;
    config.stream = false;

    RemoteLLM remote_llm(
        "127.0.0.1",
        port,
        "tool_agent.client",
        "llm.server"
    );

    galculus::scheduler::TokenBudget token_budget;
    token_budget.max_prompt_tokens = 128;
    token_budget.max_completion_tokens = 32;
    token_budget.remaining_tokens = 160;

    galculus::scheduler::TokenScheduler token_scheduler(
        token_budget
    );

    galculus::llm_scheduled::ScheduledLLMRuntime scheduled_llm(
        remote_llm,
        token_scheduler
    );

    galculus::memory::MemoryManagerConfig memory_config;
    memory_config.global_arena_size = 64 * 1024;
    memory_config.scratch_arena_size = 16 * 1024;
    
    memory_config.message_block_size = 512;
    memory_config.message_block_count = 32;

    galculus::memory::MemoryManager memory_manager(
        memory_config
    );

    memory_manager.set_agent_budget(
        "tool_agent.client",
        8 * 1024
    );

    galculus::llm_memory::MemoryAwareLLMRuntime llm(
        scheduled_llm,
        memory_manager,
        "tool_agent.client"
    );

    std::cout << "[agent] requesting memory-aware scheduled remote LLM decision\n";

    LLMResult result =
        llm.generate(
            prompt,
            config
        );

    if (!result.ok() || !result.error_message.empty())  {
        std::cerr << "[agent] LLM failed: "
                  << result.error_message << "\n";
        return 1;
    }

    std::cout << "[agent] LLM result:\n"
              << result.text << "\n";

    auto parsed =
        ToolCallParser::parse(
            result.text
        );

    if (!parsed.has_value()) {
        std::cerr << "[agent] failed to parse tool call\n";
        return 1;
    }

    ToolCall call = *parsed;

    std::cout << "[agent] parsed tool: "
              << call.name << "\n";

    std::cout << "[agent] reason: "
              << call.reason << "\n";

    std::cout << "[agent] confidence: "
              << call.confidence << "\n";

    ToolRegistry registry;

    registry.register_tool(
        "turn_on_light",
        [](const ToolCall& call) {
            std::cout << "[tool] turning on light\n";
            std::cout << "[tool] reason: "
                      << call.reason << "\n";

            return ToolResult{
                true,
                "light turned on"
            };
        }
    );

    registry.register_tool(
        "send_alert",
        [](const ToolCall& call) {
            std::cout << "[tool] sending alert\n";
            std::cout << "[tool] reason: "
                      << call.reason << "\n";

            return ToolResult{
                true,
                "alert sent"
            };
        }
    );

    registry.register_tool(
        "unlock_door",
        [](const ToolCall&) {
            std::cout << "[tool] unlocking door\n";

            return ToolResult{
                true,
                "door unlocked"
            };
        }
    );

    SafetyPolicy safety;
    safety.allow_tool("turn_on_light");
    safety.allow_tool("send_alert");
    safety.block_tool("unlock_door");

    ToolResult tool_result =
        registry.execute(
            call,
            safety
        );

    if (!tool_result.success) {
        std::cerr << "[agent] tool execution blocked/failed: "
                  << tool_result.message << "\n";
        return 1;
    }

    std::cout << "[agent] tool execution success: "
              << tool_result.message << "\n";

    const auto* agent_budget =
        memory_manager.agent_budget(
            "tool_agent.client"
        );

    if (agent_budget != nullptr) {
        std::cout << "[agent] memory used after request: "
                  << agent_budget->used_bytes()
                  << " bytes\n";

        std::cout << "[agent] memory peak usage: "
                  << agent_budget->peak_used_bytes()
                  << " bytes\n";
    }

    return 0;
}