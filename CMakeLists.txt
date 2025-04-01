cmake_minimum_required(VERSION 3.26)

add_library(embed STATIC
    embed/Exceptions.cpp
    embed/memory.cpp
    embed/OStream.cpp
    embed/math.cpp
    embed/interrupts.cpp
    embed/test.cpp
    embed/StaticArrayList.cpp

    # embed/OS/BaseTask.cpp
    # embed/OS/BaseTask_test.cpp
    # embed/OS/OS.cpp
)

target_include_directories(embed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(embed PUBLIC
    cxx_std_20
)

target_compile_options(embed PUBLIC
    -fcoroutines
)