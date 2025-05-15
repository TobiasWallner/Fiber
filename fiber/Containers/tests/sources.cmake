target_sources(fiber
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ArrayList_test.hpp
        ${CMAKE_CURRENT_LIST_DIR}/DualArrayList_test.hpp

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/ArrayList_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DualArrayList_test.cpp
)