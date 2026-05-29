add_library(galculus_llm_remote
    src/RemoteLLM.cpp
)

target_include_directories(galculus_llm_remote
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(galculus_llm_remote
    PUBLIC
        galculus::core
        galculus::llm
        galculus::llm_bridge
)

target_compile_features(galculus_llm_remote
    PUBLIC
        cxx_std_17
)

add_library(galculus::llm_remote ALIAS galculus_llm_remote)