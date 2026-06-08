add_library(galculus_llm_memory
    src/MemoryAwareLLMRuntime.cpp
)

target_include_directories(galculus_llm_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(galculus_llm_memory
    PUBLIC
        galculus_llm
        galculus_memory
)

target_compile_features(galculus_llm_memory PUBLIC cxx_std_17)