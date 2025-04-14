target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/StaticArrayList.hpp
        ${CMAKE_CURRENT_LIST_DIR}/StaticPriorityQueue.hpp
    PRIVATE
)

if(EMBED_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()