add_executable(galculus_hello_agent
    main.cpp
)

target_link_libraries(galculus_hello_agent
    PRIVATE galculus::core
)