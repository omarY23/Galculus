#include <galculus/tools/SafetyPolicy.hpp>

namespace galculus::tools {

void SafetyPolicy::allow_tool(const std::string& name) {
    allowed_tools_.insert(name);
}

void SafetyPolicy::block_tool(const std::string& name) {
    blocked_tools_.insert(name);
}

bool SafetyPolicy::is_allowed(const ToolCall& call) const {
    if (blocked_tools_.count(call.name) > 0) {
        return false;
    }

    if (allowed_tools_.empty()) {
        return true;
    }

    return allowed_tools_.count(call.name) > 0;
}

}