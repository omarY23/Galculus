add_executable(galculus_core_tests
    core_runtime_tests.cpp
)

target_link_libraries(galculus_core_tests
    PRIVATE galculus::core
)

add_test(NAME galculus_core_tests COMMAND galculus_core_tests)