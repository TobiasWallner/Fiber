target_sources(fiber
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Tick.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Duration.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TimePoint.hpp
    ${CMAKE_CURRENT_LIST_DIR}/rounding_duration_cast.hpp
    PRIVATE
        
        
)

if(FIBER_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()