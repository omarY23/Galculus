#include "galculus/llm/MockLLM.hpp"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <utility>
#include <vector>

namespace galculus::llm {

MockLLM::MockLLM(std::string fixed_response)
    : fixed_response_(std::move(fixed_response)) {}

std::string MockLLM::name() const {
    return "MockLLM";
}

static uint32_t count_words(const std::string& text) {
    std::istringstream stream(text);
    uint32_t count = 0;
    std::string word;

    while (stream >> word) {
        ++count;
    }

    return count;
}

static std::string limit_words(const std::string& text, uint32_t max_words) {
    if (max_words == 0) {
        return "";
    }

    std::istringstream stream(text);
    std::ostringstream out;
    std::string word;
    uint32_t count = 0;

    while (count < max_words && stream >> word) {
        if (count > 0) {
            out << ' ';
        }

        out << word;
        ++count;
    }

    return out.str();
}

LLMResult MockLLM::generate(
    const Prompt& prompt,
    const GenerationConfig& config
) {
    const auto start = std::chrono::steady_clock::now();

    LLMResult result;

    if (prompt.messages().empty()) {
        result.status = LLMStatus::InvalidPrompt;
        result.error_message = "Prompt is empty";
        return result;
    }

    result.text = fixed_response_;
    result.prompt_tokens = count_words(prompt.to_string());
    result.completion_tokens = count_words(result.text);

    const auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );

    result.status = LLMStatus::Ok;
    return result;
}

} // namespace galculus::llm