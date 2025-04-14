target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/memory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Allocator.hpp
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/memory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Allocator.cpp
)