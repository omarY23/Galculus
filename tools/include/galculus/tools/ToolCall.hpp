#pragma once

#include <string>

namespace galculus::tools {

struct ToolCall {
    std::string name;
    std::string reason;
    double confidence = 0.0;
};

}