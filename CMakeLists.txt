cmake_minimum_required(VERSION 3.20)

project(Galculus VERSION 0.1.0 LANGUAGES CXX)

option(GALCULUS_BUILD_EXAMPLES "Build Galculus examples" ON)
option(GALCULUS_BUILD_TESTS "Build Galculus tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(core)

if(GALCULUS_BUILD_EXAMPLES)
    add_subdirectory(examples/hello_agent)
    add_subdirectory(examples/tcp_nodes)
endif()

if(GALCULUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()