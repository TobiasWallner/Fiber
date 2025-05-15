target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ansi.hpp
        ${CMAKE_CURRENT_LIST_DIR}/OStream.hpp
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/OStream.cpp
)

if(FIBER_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()