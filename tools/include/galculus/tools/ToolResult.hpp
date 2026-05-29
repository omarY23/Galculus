#pragma once

#include <string>

namespace galculus::tools {

struct ToolResult {
    bool success = false;
    std::string message;
};

}