add_library(galculus_core
    src/runtime.cpp
    src/in_memory_transport.cpp
    src/in_memory_byte_transport.cpp
    src/protocol.cpp
    src/frame.cpp
    src/message_byte_adapter.cpp
    src/framed_message_transport.cpp
    src/tcp_byte_transport.cpp
)

target_include_directories(galculus_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_core PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(galculus_core
        PUBLIC
            ws2_32
    )
endif()

add_library(galculus::core ALIAS galculus_core)