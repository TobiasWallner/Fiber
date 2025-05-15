cmake_minimum_required(VERSION 3.25)

# Version Convention
# ------------------
# Major:     Break old behavior    1.1.0 → 2.0.0
# Minor:     Add a new function    1.0.1 → 1.1.0
# Patch:     Fix a bug             1.0.0 → 1.0.1

project(fiber VERSION 0.1.0 LANGUAGES CXX)

# Protect against multiple inclusions
if(TARGET fiber)
  return()
endif()

# Create Library
add_library(fiber STATIC)

# Configure Library
include(cmake/config.cmake)

# Add Sources
include(fiber/Core/sources.cmake)
include(fiber/Containers/sources.cmake)
include(fiber/Exceptions/sources.cmake)
include(fiber/interrupts/sources.cmake)
include(fiber/math/sources.cmake)
include(fiber/Memory/sources.cmake)
include(fiber/Chrono/sources.cmake)
include(fiber/Future/sources.cmake)
include(fiber/OS/sources.cmake)
include(fiber/OStream/sources.cmake)
include(fiber/TestFramework/sources.cmake)
include(fiber/test/sources.cmake)

include(fiber/sys_stubs/sources.cmake)

# Compiler Options
target_include_directories(fiber PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(fiber PUBLIC
    cxx_std_20
)

target_compile_options(fiber PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wsign-compare
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(fiber PUBLIC -fcoroutines)
endif()


# ================================================================================
#                                   TESTS
# ================================================================================


if(FIBER_CTEST)

    # Enable testing
    enable_testing()

    # Add test source
    add_executable(test_runner fiber/test/test_main.cpp)


    target_compile_options(test_runner PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wsign-compare
    )

    # Link to your fiber library (adjust as needed)
    target_link_libraries(test_runner PRIVATE fiber)

    # Register with CTest
    add_test(NAME fiberOS_test COMMAND test_runner)

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
    "${CMAKE_CURRENT_BINARY_DIR}/fiberConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

export(
  TARGETS fiber
  NAMESPACE fiber::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/fiberTargets.cmake"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fiberConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/fiberConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/fiber"
)

# Create an alias for clean linking
add_library(fiber::fiber ALIAS fiber)
