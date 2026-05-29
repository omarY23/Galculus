add_executable(galculus_remote_llm_client
    remote_llm_client.cpp
)

target_link_libraries(galculus_remote_llm_client
    PRIVATE
        galculus::llm
        galculus::llm_remote
)