target_sources(embed
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Tick.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Duration.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TimePoint.hpp
    ${CMAKE_CURRENT_LIST_DIR}/rounding_duration_cast.hpp
    PRIVATE
        
        
)

if(EMBED_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()