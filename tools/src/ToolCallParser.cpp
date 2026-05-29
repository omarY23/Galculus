#include <galculus/tools/ToolCallParser.hpp>

#include <sstream>
#include <string>

namespace galculus::tools {

static std::string value_after_equals(const std::string& line) {
    const auto pos = line.find('=');

    if (pos == std::string::npos) {
        return "";
    }

    return line.substr(pos + 1);
}

std::optional<ToolCall> ToolCallParser::parse(const std::string& text) {
    std::istringstream input(text);
    std::string line;

    ToolCall call;

    while (std::getline(input, line)) {
        if (line.rfind("tool=", 0) == 0) {
            call.name = value_after_equals(line);
        } else if (line.rfind("reason=", 0) == 0) {
            call.reason = value_after_equals(line);
        } else if (line.rfind("confidence=", 0) == 0) {
            call.confidence = std::stod(value_after_equals(line));
        }
    }

    if (call.name.empty()) {
        return std::nullopt;
    }

    return call;
}

}