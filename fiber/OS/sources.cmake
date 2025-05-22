target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/CoSignal.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Task.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Scheduler.hpp

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.cpp
        ${CMAKE_CURRENT_LIST_DIR}/CoSignal.cpp
        
)

if(FIBER_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()