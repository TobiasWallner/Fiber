target_sources(embed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Clock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Future.hpp
        ${CMAKE_CURRENT_LIST_DIR}/LinearScheduler.hpp
        ${CMAKE_CURRENT_LIST_DIR}/RealTimeTask.hpp
        ${CMAKE_CURRENT_LIST_DIR}/RealTimeScheduler.hpp

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Coroutine.cpp
)

if(EMBED_COMPILE_TESTS)

    include(${CMAKE_CURRENT_LIST_DIR}/tests/sources.cmake)

endif()