add_library(galculus_llm_bridge
    src/LLMMessageCodec.cpp
)

target_include_directories(galculus_llm_bridge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(galculus_llm_bridge
    PUBLIC
        galculus::core
        galculus::llm
)

target_compile_features(galculus_llm_bridge
    PUBLIC
        cxx_std_17
)

add_library(galculus::llm_bridge ALIAS galculus_llm_bridge)