target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ArrayList.hpp
        ${CMAKE_CURRENT_LIST_DIR}/PriorityQueue.hpp
    PRIVATE
)

if(EMBED_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()