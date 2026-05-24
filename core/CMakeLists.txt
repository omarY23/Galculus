add_library(galculus_core
            src/runtime.cpp
            src/in_memory_transport.cpp
            )

target_include_directories(galculus_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_core PUBLIC cxx_std_17)

add_library(galculus::core ALIAS galculus_core)
