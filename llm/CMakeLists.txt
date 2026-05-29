add_library(galculus_llm
    src/Prompt.cpp
    src/MockLLM.cpp
)

target_include_directories(galculus_llm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_llm
    PUBLIC
        cxx_std_17
)

add_library(galculus::llm ALIAS galculus_llm)

target_link_libraries(galculus_llm
    PUBLIC
        galculus::core
        galculus::llm_bridge
)