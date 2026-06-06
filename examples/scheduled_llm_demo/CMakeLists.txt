add_executable(galculus_scheduled_llm_demo
    scheduled_llm_demo.cpp
)

target_link_libraries(galculus_scheduled_llm_demo
    PRIVATE
        galculus_llm
        galculus_scheduler
        galculus_llm_scheduled
)