target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/interrupts.hpp
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/interrupts.cpp
)