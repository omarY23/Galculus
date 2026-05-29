add_library(galculus_tools
    src/ToolCallParser.cpp
    src/ToolRegistry.cpp
    src/SafetyPolicy.cpp
)

target_include_directories(galculus_tools
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_tools
    PUBLIC
        cxx_std_17
)

add_library(galculus::tools ALIAS galculus_tools)