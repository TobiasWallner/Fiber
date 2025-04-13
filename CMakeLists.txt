cmake_minimum_required(VERSION 3.26)

add_library(embed STATIC
    embed/memory.cpp
    embed/OStream.cpp
    embed/math.cpp
    embed/interrupts.cpp
    embed/test.cpp
    embed/StaticArrayList.cpp
    embed/Exceptions.cpp

    #embed/OS/BaseTask.cpp
    
    #embed/OS/OS.cpp

    # tests
    #embed/OS/BaseTask_test.cpp
    embed/OS2/Coroutine.cpp
    embed/OS2/Coroutine_test.cpp

    embed/OS/Clock_test.cpp
)

option(
    USE_EMBED_SYS_STUBS 
    "Enables system stubs that prevent the usage of unnecessary standard library features and massively reduces binary size"
    OFF)

if(NOT USE_EMBED_SYS_STUBS)
    message(STATUS "embed Tip:\n"
                   "  Enabling: `USE_EMBED_SYS_STUBS`, and reduce binary size for your bare-metal embedded system"
                   "  and linking to: `embed_sys_stubs`\n")
endif()

if(USE_EMBED_SYS_STUBS)
    add_library(embed_sys_stubs OBJECT
        embed/sys_stubs/sys_stubs.cpp
    )

    target_link_options(embed_sys_stubs PUBLIC
        -Wl,--wrap=atexit
    )
endif()

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
option(EMBED_ASSERTS_AS_ASSUME "Use compiler intrinsics that asume values instead of unused asserts - for more optimisations (like __builtin_assume, or a different, or none - depending on the compiler used)" OFF)
option(EMBED_USE_EXCEPTION_CALLBACKS "Use user-defined callbacks for exceptions instead of throwing them" OFF)

# === ANSI formatting ===
option(EMBED_DISABLE_ANSI_CODES "Disable ANSI color escape codes in output" OFF)

# === Number formating ===
option(EMBED_FMT_MINIMAL "Disables most numerical runtime string formating. Allows setting macros for concrete formating styles" OFF)

option(EMBED_BOOL_TO_TEXT "Enables bool formating as text instead of numbers." ON)

option(EMBED_FMT_DECIMALS_1 "Enables 1 decimal place" OFF)
option(EMBED_FMT_DECIMALS_2 "Enables 2 decimal places" OFF)
option(EMBED_FMT_DECIMALS_3 "Enables 3 decimal places" OFF)
option(EMBED_FMT_DECIMALS_4 "Enables 4 decimal places" OFF)
option(EMBED_FMT_DECIMALS_5 "Enables 5 decimal places" OFF)
option(EMBED_FMT_DECIMALS_6 "Enables 6 decimal places" OFF)

# Count how many levels are enabled
set(_embed_fmt_decimals_count 0)

foreach(_level
    EMBED_FMT_DECIMALS_1
    EMBED_FMT_DECIMALS_2
    EMBED_FMT_DECIMALS_3
    EMBED_FMT_DECIMALS_4
    EMBED_FMT_DECIMALS_5
    EMBED_FMT_DECIMALS_6
)
    if(${_level})
        math(EXPR _embed_fmt_decimals_count "${_embed_fmt_decimals_count} + 1")
    endif()
endforeach()

# Error if more than one is enabled
if(_embed_fmt_decimals_count GREATER 1)
    message(FATAL_ERROR
        "embed: Multiple decimal places are enabled!\n"
        "  Please enable only ONE of the following:\n"
        "    - EMBED_FMT_DECIMALS_1         = ${EMBED_FMT_DECIMALS_1}\n"
        "    - EMBED_FMT_DECIMALS_2         = ${EMBED_FMT_DECIMALS_2}\n"
        "    - EMBED_FMT_DECIMALS_3         = ${EMBED_FMT_DECIMALS_3}\n"
        "    - EMBED_FMT_DECIMALS_4         = ${EMBED_FMT_DECIMALS_4}\n"
        "    - EMBED_FMT_DECIMALS_5         = ${EMBED_FMT_DECIMALS_5}\n"
        "    - EMBED_FMT_DECIMALS_6         = ${EMBED_FMT_DECIMALS_6}\n"
    )
endif()

# Default to EMBED_ASSERTION_LEVEL_CRITICAL if none set
if(_embed_fmt_decimals_count EQUAL 0)
    set(EMBED_FMT_DECIMALS_3 ON)
    message(STATUS "embed: No decimal place numer set. Defaulting to EMBED_FMT_DECIMALS_3 = ON")
endif()


option(EMBED_FMT_FLOAT_REP_SCI "Enables scientific representation" OFF)
option(EMBED_FMT_FLOAT_REP_ENG "Enables engineering representation" OFF)
option(EMBED_FMT_FLOAT_REP_FULL "Enables floating point representation" OFF)

# Count how many levels are enabled
set(_embed_fmt_float_rep_count 0)

foreach(_level
    EMBED_FMT_FLOAT_REP_SCI
    EMBED_FMT_FLOAT_REP_ENG
    EMBED_FMT_FLOAT_REP_FULL
)
    if(${_level})
        math(EXPR _embed_fmt_float_rep_count "${_embed_fmt_float_rep_count} + 1")
    endif()
endforeach()

# Error if more than one is enabled
if(_embed_fmt_float_rep_count GREATER 1)
    message(FATAL_ERROR
        "embed: Multiple float representations are enabled!\n"
        "  Please enable only ONE of the following:\n"
        "    - EMBED_FMT_FLOAT_REP_SCI         = ${EMBED_FMT_FLOAT_REP_SCI}\n"
        "    - EMBED_FMT_FLOAT_REP_ENG         = ${EMBED_FMT_FLOAT_REP_ENG}\n"
        "    - EMBED_FMT_FLOAT_REP_FULL        = ${EMBED_FMT_FLOAT_REP_FULL}\n"
    )
endif()

# Default to EMBED_ASSERTION_LEVEL_CRITICAL if none set
if(_embed_fmt_float_rep_count EQUAL 0)
    set(EMBED_FMT_FLOAT_REP_ENG ON)
    message(STATUS "embed: No floating point representation set. Defaulting to EMBED_FMT_FLOAT_REP_ENG = ON")
endif()

option(EMBED_FMT_PAD_SIGN "Enables sign padding" OFF)
option(EMBED_FMT_DOT_AS_COMMA "Use dot '.' as comma and ',' as thousands for numbers: 123,456.789 | if OFF: Otherwise" ON)
option(EMBED_FMT_THOUSANDS "Enables the use of thousands (depends on *_AS_COMMA): 123,456,789" OFF)
option(EMBED_FMT_FORCE_COMMA "if set, makes sure that a comma is always printed for floating point numbers" ON)
option(EMBED_FMT_FORCE_DECIMALS "if set will make sure that the set ammount of decimals will always be printed, even if zero" ON)
option(EMBED_FMT_FORCE_SIGN "forces a sign, even if the number is positive" OFF)
option(EMBED_FMT_FORCE_EXPONENT "forces an exponent, even if the exponent is zero" OFF)
option(EMBED_FMT_FORCE_EXPONENT_SIGN "forces a sign on the exponent, even if the exponent is positive" OFF)

# === Show currently used definitions ===

message(STATUS "embed Options:")
message(STATUS "  EMBED_DISABLE_ASSERTIONS         = ${EMBED_DISABLE_ASSERTIONS}")
message(STATUS "  EMBED_ASSERTION_LEVEL_CRITICAL   = ${EMBED_ASSERTION_LEVEL_CRITICAL}")
message(STATUS "  EMBED_ASSERTION_LEVEL_O1         = ${EMBED_ASSERTION_LEVEL_O1}")
message(STATUS "  EMBED_ASSERTION_LEVEL_FULL       = ${EMBED_ASSERTION_LEVEL_FULL}")
message(STATUS "  EMBED_ASSERTS_AS_ASSUME          = ${EMBED_ASSERTS_AS_ASSUME}")
message(STATUS "  EMBED_USE_EXCEPTION_CALLBACKS    = ${EMBED_USE_EXCEPTION_CALLBACKS}")
message(STATUS "  EMBED_DISABLE_ANSI_CODES         = ${EMBED_DISABLE_ANSI_CODES}")
message(STATUS "  EMBED_FMT_MINIMAL                = ${EMBED_FMT_MINIMAL}")
if(EMBED_FMT_MINIMAL)
    message(STATUS "    EMBED_BOOL_TO_TEXT             = ${EMBED_BOOL_TO_TEXT}")
    message(STATUS "    EMBED_FMT_FLOAT_REP_SCI        = ${EMBED_FMT_FLOAT_REP_SCI}")
    message(STATUS "    EMBED_FMT_FLOAT_REP_ENG        = ${EMBED_FMT_FLOAT_REP_ENG}")
    message(STATUS "    EMBED_FMT_FLOAT_REP_FULL       = ${EMBED_FMT_FLOAT_REP_FULL}")
    message(STATUS "    EMBED_FMT_DECIMALS_1           = ${EMBED_FMT_DECIMALS_1}")
    message(STATUS "    EMBED_FMT_DECIMALS_2           = ${EMBED_FMT_DECIMALS_2}")
    message(STATUS "    EMBED_FMT_DECIMALS_3           = ${EMBED_FMT_DECIMALS_3}")
    message(STATUS "    EMBED_FMT_DECIMALS_4           = ${EMBED_FMT_DECIMALS_4}")
    message(STATUS "    EMBED_FMT_DECIMALS_5           = ${EMBED_FMT_DECIMALS_5}")
    message(STATUS "    EMBED_FMT_DECIMALS_6           = ${EMBED_FMT_DECIMALS_6}")
    message(STATUS "    EMBED_FMT_DOT_AS_COMMA         = ${EMBED_FMT_DOT_AS_COMMA}")
    message(STATUS "    EMBED_FMT_PAD_SIGN             = ${EMBED_FMT_PAD_SIGN}")
    message(STATUS "    EMBED_FMT_THOUSANDS            = ${EMBED_FMT_THOUSANDS}")
    message(STATUS "    EMBED_FMT_FORCE_COMMA          = ${EMBED_FMT_FORCE_COMMA}")
    message(STATUS "    EMBED_FMT_FORCE_DECIMALS       = ${EMBED_FMT_FORCE_DECIMALS}")
    message(STATUS "    EMBED_FMT_FORCE_SIGN           = ${EMBED_FMT_FORCE_SIGN}")
    message(STATUS "    EMBED_FMT_FORCE_EXPONENT       = ${EMBED_FMT_FORCE_EXPONENT}")
    message(STATUS "    EMBED_FMT_FORCE_EXPONENT_SIGN  = ${EMBED_FMT_FORCE_EXPONENT_SIGN}")
endif()

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

    $<$<BOOL:${EMBED_FMT_MINIMAL}>:EMBED_FMT_MINIMAL>
    $<$<BOOL:${EMBED_BOOL_TO_TEXT}>:EMBED_BOOL_TO_TEXT>
    $<$<BOOL:${EMBED_FMT_DECIMALS_1}>:EMBED_FMT_DECIMALS_1>
    $<$<BOOL:${EMBED_FMT_DECIMALS_2}>:EMBED_FMT_DECIMALS_2>
    $<$<BOOL:${EMBED_FMT_DECIMALS_3}>:EMBED_FMT_DECIMALS_3>
    $<$<BOOL:${EMBED_FMT_DECIMALS_4}>:EMBED_FMT_DECIMALS_4>
    $<$<BOOL:${EMBED_FMT_DECIMALS_5}>:EMBED_FMT_DECIMALS_5>
    $<$<BOOL:${EMBED_FMT_DECIMALS_6}>:EMBED_FMT_DECIMALS_6>

    $<$<BOOL:${EMBED_FMT_FLOAT_REP_SCI}>:EMBED_FMT_FLOAT_REP_SCI>
    $<$<BOOL:${EMBED_FMT_FLOAT_REP_ENG}>:EMBED_FMT_FLOAT_REP_ENG>
    $<$<BOOL:${EMBED_FMT_FLOAT_REP_FULL}>:EMBED_FMT_FLOAT_REP_FULL>

    $<$<BOOL:${EMBED_FMT_PAD_SIGN}>:EMBED_FMT_PAD_SIGN>
    $<$<BOOL:${EMBED_FMT_DOT_AS_COMMA}>:EMBED_FMT_DOT_AS_COMMA>
    $<$<BOOL:${EMBED_FMT_THOUSANDS}>:EMBED_FMT_THOUSANDS>
    $<$<BOOL:${EMBED_FMT_FORCE_COMMA}>:EMBED_FMT_FORCE_COMMA>
    $<$<BOOL:${EMBED_FMT_FORCE_DECIMALS}>:EMBED_FMT_FORCE_DECIMALS>
    $<$<BOOL:${EMBED_FMT_FORCE_SIGN}>:EMBED_FMT_FORCE_SIGN>
    $<$<BOOL:${EMBED_FMT_FORCE_EXPONENT_SIGN}>:EMBED_FMT_FORCE_EXPONENT_SIGN>
)

target_compile_features(embed PUBLIC
    cxx_std_20
)

target_compile_options(embed PUBLIC
    -fcoroutines
)