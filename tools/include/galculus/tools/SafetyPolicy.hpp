#pragma once

#include <string>
#include <unordered_set>

#include <galculus/tools/ToolCall.hpp>

namespace galculus::tools {

class SafetyPolicy {
public:
    void allow_tool(const std::string& name);
    void block_tool(const std::string& name);

    bool is_allowed(const ToolCall& call) const;

private:
    std::unordered_set<std::string> allowed_tools_;
    std::unordered_set<std::string> blocked_tools_;
};

}