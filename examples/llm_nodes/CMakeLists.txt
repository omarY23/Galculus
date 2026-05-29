add_executable(galculus_llm_server
    llm_server.cpp
)

target_link_libraries(galculus_llm_server
    PRIVATE
        galculus::core
        galculus::llm
        galculus::llm_bridge
)

add_executable(galculus_llm_client
    llm_client.cpp
)

target_link_libraries(galculus_llm_client
    PRIVATE
        galculus::core
        galculus::llm
        galculus::llm_bridge
)