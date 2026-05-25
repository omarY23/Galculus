#include "galculus/llm/Prompt.hpp"
#include "galculus/llm/GenerationConfig.hpp"
#include "galculus/llm/LLMResult.hpp"
#include "galculus/llm/MockLLM.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace galculus::llm;

static void test_prompt_stores_messages() {
    Prompt prompt;
    prompt.add_system("You are safe.");
    prompt.add_user("Hello.");

    assert(prompt.messages().size() == 2);
    assert(prompt.messages()[0].role == PromptRole::System);
    assert(prompt.messages()[0].content == "You are safe.");
    assert(prompt.messages()[1].role == PromptRole::User);
    assert(prompt.messages()[1].content == "Hello.");
}

static void test_prompt_serializes_to_string() {
    Prompt prompt;
    prompt.add_system("You are safe.");
    prompt.add_user("Hello.");

    const std::string expected =
        "system: You are safe.\n"
        "user: Hello.\n";

    assert(prompt.to_string() == expected);
}

static void test_mock_llm_returns_fixed_response() {
    Prompt prompt;
    prompt.add_user("Motion detected.");

    GenerationConfig config;
    config.max_tokens = 32;

    MockLLM llm("turn_on_light");

    LLMResult result = llm.generate(prompt, config);

    assert(result.ok());
    assert(result.text == "turn_on_light");
}

static void test_mock_llm_rejects_empty_prompt() {
    Prompt prompt;

    GenerationConfig config;
    MockLLM llm("mock response");

    LLMResult result = llm.generate(prompt, config);

    assert(!result.ok());
    assert(result.status == LLMStatus::InvalidPrompt);
    assert(!result.error_message.empty());
}

static void test_mock_llm_respects_max_tokens_as_word_limit() {
    Prompt prompt;
    prompt.add_user("Say something.");

    GenerationConfig config;
    config.max_tokens = 2;

    MockLLM llm("one two three four");

    LLMResult result = llm.generate(prompt, config);

    assert(result.ok());
    assert(result.text == "one two");
    assert(result.completion_tokens == 2);
}

static void test_mock_llm_name() {
    MockLLM llm;
    assert(llm.name() == "MockLLM");
}

int main() {
    test_prompt_stores_messages();
    test_prompt_serializes_to_string();
    test_mock_llm_returns_fixed_response();
    test_mock_llm_rejects_empty_prompt();
    test_mock_llm_respects_max_tokens_as_word_limit();
    test_mock_llm_name();

    std::cout << "All LLM runtime tests passed.\n";
    return 0;
}