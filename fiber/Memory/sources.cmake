target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/memory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/StaticLinearAllocator.hpp
        ${CMAKE_CURRENT_LIST_DIR}/StackAllocator.hpp

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/memory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/StackAllocator.cpp
)