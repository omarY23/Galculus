add_library(galculus_llm_scheduled
    src/ScheduledLLMRuntime.cpp
)

target_include_directories(galculus_llm_scheduled
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(galculus_llm_scheduled
    PUBLIC
        galculus_llm
        galculus_scheduler
)

target_compile_features(galculus_llm_scheduled PUBLIC cxx_std_17)