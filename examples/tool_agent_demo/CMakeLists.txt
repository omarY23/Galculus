add_executable(galculus_tool_agent_client
    tool_agent_client.cpp
)

target_link_libraries(galculus_tool_agent_client
    PRIVATE
        galculus_core
        galculus_llm
        galculus_llm_bridge
        galculus_llm_remote
        galculus_tools
        galculus_scheduler
        galculus_llm_scheduled
        galculus_memory
        galculus_llm_memory
)