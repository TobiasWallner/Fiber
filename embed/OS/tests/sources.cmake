target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Clock_test.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine_test.cpp

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Clock_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine_test.cpp

)
