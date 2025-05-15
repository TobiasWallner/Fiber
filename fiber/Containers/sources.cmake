target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ArrayList.hpp
        ${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.hpp
    PRIVATE
)

if(FIBER_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()