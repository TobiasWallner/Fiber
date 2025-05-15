target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/memory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/StaticLinearAllocator.hpp
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/memory.cpp
)