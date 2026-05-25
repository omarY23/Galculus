#include "galculus/llm/Prompt.hpp"

#include <sstream>

namespace galculus::llm {

static const char* role_to_string(PromptRole role) {
    switch (role) {
        case PromptRole::System: return "system";
        case PromptRole::User: return "user";
        case PromptRole::Assistant: return "assistant";
        case PromptRole::Tool: return "tool";
        default: return "unknown";
    }
}

void Prompt::add_system(std::string text) {
    messages_.push_back({PromptRole::System, std::move(text)});
}

void Prompt::add_user(std::string text) {
    messages_.push_back({PromptRole::User, std::move(text)});
}

void Prompt::add_assistant(std::string text) {
    messages_.push_back({PromptRole::Assistant, std::move(text)});
}

void Prompt::add_tool(std::string text) {
    messages_.push_back({PromptRole::Tool, std::move(text)});
}

const std::vector<PromptMessage>& Prompt::messages() const {
    return messages_;
}

std::string Prompt::to_string() const {
    std::ostringstream out;

    for (const auto& msg : messages_) {
        out << role_to_string(msg.role) << ": " << msg.content << '\n';
    }

    return out.str();
}

} // namespace galculus::llm