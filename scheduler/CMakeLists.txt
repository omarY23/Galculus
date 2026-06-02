add_library(galculus_scheduler
    src/TokenScheduler.cpp
)

target_include_directories(galculus_scheduler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_scheduler PUBLIC cxx_std_17)