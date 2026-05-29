#include <galculus/tools/ToolCallParser.hpp>
#include <galculus/tools/ToolRegistry.hpp>
#include <galculus/tools/SafetyPolicy.hpp>

#include <cassert>
#include <iostream>

using namespace galculus::tools;

static void test_parse_tool_call() {
    const std::string text =
        "tool=turn_on_light\n"
        "reason=motion detected near door\n"
        "confidence=0.82\n";

    auto call = ToolCallParser::parse(text);

    assert(call.has_value());
    assert(call->name == "turn_on_light");
    assert(call->reason == "motion detected near door");
    assert(call->confidence == 0.82);
}

static void test_execute_allowed_tool() {
    ToolRegistry registry;

    registry.register_tool(
        "turn_on_light",
        [](const ToolCall&) {
            return ToolResult{true, "light turned on"};
        }
    );

    SafetyPolicy safety;
    safety.allow_tool("turn_on_light");

    ToolCall call;
    call.name = "turn_on_light";

    auto result = registry.execute(call, safety);

    assert(result.success);
    assert(result.message == "light turned on");
}

static void test_blocked_tool() {
    ToolRegistry registry;

    registry.register_tool(
        "unlock_door",
        [](const ToolCall&) {
            return ToolResult{true, "door unlocked"};
        }
    );

    SafetyPolicy safety;
    safety.block_tool("unlock_door");

    ToolCall call;
    call.name = "unlock_door";

    auto result = registry.execute(call, safety);

    assert(!result.success);
}

int main() {
    test_parse_tool_call();
    test_execute_allowed_tool();
    test_blocked_tool();

    std::cout << "All tool runtime tests passed.\n";
    return 0;
}