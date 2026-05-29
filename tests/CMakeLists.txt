add_executable(galculus_core_tests
    core_runtime_tests.cpp
)

target_link_libraries(galculus_core_tests
    PRIVATE galculus::core
)

add_test(NAME galculus_core_tests COMMAND galculus_core_tests)


add_executable(galculus_protocol_tests
    protocol_tests.cpp
)

target_link_libraries(galculus_protocol_tests
    PRIVATE galculus::core
)

add_test(NAME galculus_protocol_tests COMMAND galculus_protocol_tests)


add_executable(galculus_byte_transport_tests
    byte_transport_tests.cpp
)

target_link_libraries(galculus_byte_transport_tests
    PRIVATE galculus::core
)

add_test(NAME galculus_byte_transport_tests COMMAND galculus_byte_transport_tests)


add_executable(galculus_frame_tests
    frame_tests.cpp
)

target_link_libraries(galculus_frame_tests
    PRIVATE galculus::core
)

add_test(NAME galculus_frame_tests COMMAND galculus_frame_tests)

add_executable(galculus_message_byte_adapter_tests
    message_byte_adapter_tests.cpp
)

target_link_libraries(galculus_message_byte_adapter_tests
    PRIVATE galculus::core
)

add_test(
    NAME galculus_message_byte_adapter_tests
    COMMAND galculus_message_byte_adapter_tests
)

add_executable(galculus_framed_message_transport_tests
    framed_message_transport_tests.cpp
)

target_link_libraries(galculus_framed_message_transport_tests
    PRIVATE galculus::core
)

add_test(
    NAME galculus_framed_message_transport_tests
    COMMAND galculus_framed_message_transport_tests
)


add_executable(galculus_runtime_framed_transport_tests
    runtime_framed_transport_tests.cpp
)

target_link_libraries(galculus_runtime_framed_transport_tests
    PRIVATE galculus::core
)

add_test(
    NAME galculus_runtime_framed_transport_tests
    COMMAND galculus_runtime_framed_transport_tests
)

add_executable(galculus_tcp_byte_transport_tests
    tcp_byte_transport_tests.cpp
)

target_link_libraries(galculus_tcp_byte_transport_tests
    PRIVATE galculus::core
)

add_test(
    NAME galculus_tcp_byte_transport_tests
    COMMAND galculus_tcp_byte_transport_tests
)
add_executable(test_llm_runtime
    test_llm_runtime.cpp
)

target_link_libraries(test_llm_runtime PRIVATE
    galculus::llm
)

add_test(NAME test_llm_runtime COMMAND test_llm_runtime)

add_executable(galculus_llm_message_codec_tests
    llm_message_codec_tests.cpp
)

target_link_libraries(galculus_llm_message_codec_tests
    PRIVATE
        galculus::llm_bridge
)

add_test(
    NAME galculus_llm_message_codec_tests
    COMMAND galculus_llm_message_codec_tests
)

add_executable(galculus_tool_runtime_tests
    tool_runtime_tests.cpp
)

target_link_libraries(galculus_tool_runtime_tests
    PRIVATE
        galculus::tools
)

add_test(
    NAME galculus_tool_runtime_tests
    COMMAND galculus_tool_runtime_tests
)