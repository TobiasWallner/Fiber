target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine_test.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Scheduler_test.hpp
        
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Scheduler_test.cpp
)
