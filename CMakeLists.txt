cmake_minimum_required(VERSION 3.25)

# Version Convention
# ------------------
# Major:     Break old behavior    1.1.0 → 2.0.0
# Minor:     Add a new function    1.0.1 → 1.1.0
# Patch:     Fix a bug             1.0.0 → 1.0.1

project(embed VERSION 0.1.0 LANGUAGES CXX)

# Protect against multiple inclusions
if(TARGET embed)
  return()
endif()

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
include(embed/TestFramework/sources.cmake)
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
    -Wall
    -Wextra
    -Wsign-compare
    #-fmax-errors=3
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(embed PUBLIC -fcoroutines)
endif()


# ================================================================================
#                                   TESTS
# ================================================================================


if(EMBED_CTEST)

    # Enable testing
    enable_testing()

    # Add test source
    add_executable(test_runner embed/test/test_main.cpp)

    # Link to your embed library (adjust as needed)
    target_link_libraries(test_runner PRIVATE embed)

    # Register with CTest
    add_test(NAME embedOS_test COMMAND test_runner)

endif()

# ================================================================================
#                  find_package() support without installing
# ================================================================================

# Why not installing?
# -------------------
# This library is primarily meant for embedded systems and embedded devs often switch 
# MCUs and CPUs. Having multiple installs for multiple different targets is just not
# sufficient

# Export target for find_package() discovery
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/embedConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

export(
  TARGETS embed
  NAMESPACE embed::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/embedTargets.cmake"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/embedConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/embedConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/embed"
)

# Create an alias for clean linking
add_library(embed::embed ALIAS embed)
