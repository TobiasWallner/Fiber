cmake_minimum_required(VERSION 3.26)

add_library(embed STATIC
    embed/memory.cpp
    embed/OStream.cpp
    embed/math.cpp
    embed/interrupts.cpp
    embed/test.cpp
    embed/StaticArrayList.cpp
    embed/Exceptions.cpp

    embed/OS/BaseTask.cpp
    embed/OS/BaseTask_test.cpp
    #embed/OS/OS.cpp
)

target_include_directories(embed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# === Assertion system switches ===
option(EMBED_DISABLE_ASSERTIONS         "Disable all embedOS assertions" OFF)
option(EMBED_ASSERTION_LEVEL_CRITICAL   "Enable critical level assertions" OFF)
option(EMBED_ASSERTION_LEVEL_O1         "Enable O[1] cost development assertions + critical" OFF)
option(EMBED_ASSERTION_LEVEL_FULL       "Enable full deep validation assertions + O[1] + full" OFF)

# === Error Checking ===

# Count how many levels are enabled
set(_embed_assert_level_count 0)

foreach(_level
    EMBED_DISABLE_ASSERTIONS
    EMBED_ASSERTION_LEVEL_CRITICAL
    EMBED_ASSERTION_LEVEL_O1
    EMBED_ASSERTION_LEVEL_FULL
)
    if(${_level})
        math(EXPR _embed_assert_level_count "${_embed_assert_level_count} + 1")
    endif()
endforeach()

# Error if more than one is enabled
if(_embed_assert_level_count GREATER 1)
    message(FATAL_ERROR
        "embed: Multiple assertion levels are enabled!\n"
        "  Please enable only ONE of the following:\n"
        "    - EMBED_DISABLE_ASSERTIONS         = ${EMBED_DISABLE_ASSERTIONS}\n"
        "    - EMBED_ASSERTION_LEVEL_CRITICAL   = ${EMBED_ASSERTION_LEVEL_CRITICAL}\n"
        "    - EMBED_ASSERTION_LEVEL_O1         = ${EMBED_ASSERTION_LEVEL_O1}\n"
        "    - EMBED_ASSERTION_LEVEL_FULL       = ${EMBED_ASSERTION_LEVEL_FULL}\n"
    )
endif()

# Default to EMBED_ASSERTION_LEVEL_CRITICAL if none set
if(_embed_assert_level_count EQUAL 0)
    set(EMBED_ASSERTION_LEVEL_CRITICAL ON)
    message(STATUS "embed: No assertion level set. Defaulting to EMBED_ASSERTION_LEVEL_CRITICAL = ON")
endif()

# === Assertion behavior ===
option(EMBED_ASSERTS_AS_ASSUME          "Use compiler intrinsics that asume values instead of unused asserts - for more optimisations (like __builtin_assume, or a different, or none - depending on the compiler used)" OFF)
option(EMBED_USE_EXCEPTION_CALLBACKS    "Use user-defined callbacks for exceptions instead of throwing them" OFF)

# === ANSI formatting ===
option(EMBED_DISABLE_ANSI_CODES         "Disable ANSI color escape codes in output" OFF)

# === Show currently used definitions ===

message(STATUS "embed Options:")
message(STATUS "  EMBED_DISABLE_ASSERTIONS         = ${EMBED_DISABLE_ASSERTIONS}")
message(STATUS "  EMBED_ASSERTION_LEVEL_CRITICAL   = ${EMBED_ASSERTION_LEVEL_CRITICAL}")
message(STATUS "  EMBED_ASSERTION_LEVEL_O1         = ${EMBED_ASSERTION_LEVEL_O1}")
message(STATUS "  EMBED_ASSERTION_LEVEL_FULL       = ${EMBED_ASSERTION_LEVEL_FULL}")
message(STATUS "  EMBED_ASSERTS_AS_ASSUME          = ${EMBED_ASSERTS_AS_ASSUME}")
message(STATUS "  EMBED_USE_EXCEPTION_CALLBACKS    = ${EMBED_USE_EXCEPTION_CALLBACKS}")
message(STATUS "  EMBED_DISABLE_ANSI_CODES         = ${EMBED_DISABLE_ANSI_CODES}")

# === Apply definitions ===

# These should be attached to your core embedOS target
# Replace `embed` with your actual library target name
target_compile_definitions(embed PUBLIC
    $<$<BOOL:${EMBED_DISABLE_ASSERTIONS}>:EMBED_DISABLE_ASSERTIONS>
    $<$<BOOL:${EMBED_ASSERTION_LEVEL_CRITICAL}>:EMBED_ASSERTION_LEVEL_CRITICAL>
    $<$<BOOL:${EMBED_ASSERTION_LEVEL_O1}>:EMBED_ASSERTION_LEVEL_O1>
    $<$<BOOL:${EMBED_ASSERTION_LEVEL_FULL}>:EMBED_ASSERTION_LEVEL_FULL>
    $<$<BOOL:${EMBED_ASSERTS_AS_ASSUME}>:EMBED_ASSERTS_AS_ASSUME>
    $<$<BOOL:${EMBED_USE_EXCEPTION_CALLBACKS}>:EMBED_USE_EXCEPTION_CALLBACKS>
    $<$<BOOL:${EMBED_DISABLE_ANSI_CODES}>:EMBED_DISABLE_ANSI_CODES>
)

target_compile_features(embed PUBLIC
    cxx_std_20
)

target_compile_options(embed PUBLIC
    -fcoroutines
)