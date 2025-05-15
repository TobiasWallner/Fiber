if(FIBER_COMPILE_TESTS)
    target_sources(fiber
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/test.hpp
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/test.cpp
    )
endif()