add_executable(galculus_tool_agent_client
    tool_agent_client.cpp
)

target_link_libraries(galculus_tool_agent_client
    PRIVATE
        galculus::llm
        galculus::llm_remote
        galculus::tools
        galculus_scheduler
        galculus_llm_scheduled
)