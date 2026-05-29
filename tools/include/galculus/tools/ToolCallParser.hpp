#pragma once

#include <optional>
#include <string>

#include <galculus/tools/ToolCall.hpp>

namespace galculus::tools {

class ToolCallParser {
public:
    static std::optional<ToolCall> parse(const std::string& text);
};

}