add_library(galculus_memory
    src/MemoryArena.cpp
    src/FixedBlockPool.cpp
    src/ScratchBuffer.cpp
    src/AgentMemoryBudget.cpp
    src/MemoryManager.cpp
    src/ScopedMemoryReservation.cpp
)

target_include_directories(galculus_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(galculus_memory PUBLIC cxx_std_17)