cmake_minimum_required(VERSION 3.26)

# Create Library
add_library(embed STATIC)

# Configure Library
include(config.cmake)

# Add Sources
include(embed/Core/sources.cmake)
include(embed/Containers/sources.cmake)
include(embed/Exceptions/sources.cmake)
include(embed/interrupts/sources.cmake)
include(embed/math/sources.cmake)
include(embed/Memory/sources.cmake)
include(embed/OS/sources.cmake)
include(embed/OStream/sources.cmake)
include(embed/test/sources.cmake)

include(embed/sys_stubs/sources.cmake)

# Compiler Options
target_include_directories(embed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(embed PUBLIC
    cxx_std_20
)

target_compile_options(embed PUBLIC
    -O3
    -fcoroutines
)