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
    #-O3
    -fcoroutines
    -Wall
    -Wsign-compare
)

# ================================================================================
#                                   TESTS
# ================================================================================

if(EMBED_COMPILE_TESTS)

add_executable(embed_test
    test.cpp
)

target_link_libraries(embed_test PRIVATE
    embed
)

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
