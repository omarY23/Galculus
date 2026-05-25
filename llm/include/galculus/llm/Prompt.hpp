#pragma once
#include <vector>
#include <string>

namespace galculus::llm {

    enum class PromptRole {
        System,
        User,
        Assistant,
        Tool
    };
    
    struct PromptMessage {
        PromptRole role;
        std::string content;
    };
    
    class Prompt {
    public:
        void add_system(std::string text);
        void add_user(std::string text);
        void add_assistant(std::string text);
        void add_tool(std::string text);
    
        const std::vector<PromptMessage>& messages() const;
    
        std::string to_string() const;
    
    private:
        std::vector<PromptMessage> messages_;
    };
} // namespace galculus::llm