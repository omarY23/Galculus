#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <galculus/tools/SafetyPolicy.hpp>
#include <galculus/tools/ToolCall.hpp>
#include <galculus/tools/ToolResult.hpp>

namespace galculus::tools {

using ToolHandler = std::function<ToolResult(const ToolCall&)>;

class ToolRegistry {
public:
    void register_tool(
        const std::string& name,
        ToolHandler handler
    );

    bool has_tool(const std::string& name) const;

    ToolResult execute(
        const ToolCall& call,
        const SafetyPolicy& safety
    ) const;

private:
    std::unordered_map<std::string, ToolHandler> tools_;
};

}