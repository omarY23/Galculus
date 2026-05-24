add_executable(galculus_tcp_server
    server.cpp
)

target_link_libraries(galculus_tcp_server
    PRIVATE galculus::core
)

add_executable(galculus_tcp_client
    client.cpp
)

target_link_libraries(galculus_tcp_client
    PRIVATE galculus::core
)