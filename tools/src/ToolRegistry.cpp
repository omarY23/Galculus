#include <galculus/tools/ToolRegistry.hpp>

namespace galculus::tools {

void ToolRegistry::register_tool(
    const std::string& name,
    ToolHandler handler
) {
    tools_[name] = std::move(handler);
}

bool ToolRegistry::has_tool(const std::string& name) const {
    return tools_.count(name) > 0;
}

ToolResult ToolRegistry::execute(
    const ToolCall& call,
    const SafetyPolicy& safety
) const {
    if (!has_tool(call.name)) {
        return {
            false,
            "Tool not registered: " + call.name
        };
    }

    if (!safety.is_allowed(call)) {
        return {
            false,
            "Tool blocked by safety policy: " + call.name
        };
    }

    return tools_.at(call.name)(call);
}

}