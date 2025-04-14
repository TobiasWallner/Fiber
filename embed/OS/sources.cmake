target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Clock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Future.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Scheduler.hpp
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Scheduler.cpp
)

if(EMBED_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()