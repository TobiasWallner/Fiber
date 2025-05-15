target_sources(fiber
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Future.hpp

    PRIVATE
        
        
)

if(FIBER_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()