#pragma once

#include <string>

namespace galculus::core {

enum class ErrorCode {
    None,
    InvalidArgument,
    AgentAlreadyExists,
    AgentNotFound,
    RuntimeNotRunning,
    TransportError
};

struct Result {
    bool ok{true};
    ErrorCode code{ErrorCode::None};
    std::string message;

    static Result success() {
        return Result{};
    }

    static Result failure(ErrorCode code, std::string message) {
        Result result;
        result.ok = false;
        result.code = code;
        result.message = std::move(message);
        return result;
    }
};

} // namespace galculus::core