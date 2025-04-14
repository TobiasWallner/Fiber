
# ================================================================================
#                                Compile Tests
# ================================================================================

option(EMBED_COMPILE_TESTS "If ON compiles the test sources" OFF)

# ================================================================================
#                                system stubs
# ================================================================================

option(
    EMBED_USE_EMBED_SYS_STUBS 
    "Enables system stubs that prevent the usage of unnecessary standard library features and massively reduces binary size"
    OFF)


# ================================================================================
#                                Assertions
# ================================================================================

# Assertion Levels
# ----------------
option(EMBED_DISABLE_ASSERTIONS         "Disable all embedOS assertions" OFF)
option(EMBED_ASSERTION_LEVEL_CRITICAL   "Enable critical level assertions" OFF)
option(EMBED_ASSERTION_LEVEL_O1         "Enable O[1] cost development assertions + critical" OFF)
option(EMBED_ASSERTION_LEVEL_FULL       "Enable full deep validation assertions + O[1] + full" OFF)

# Check that maximal 1 is set
# ---------------------------
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
# -----------------------------------------------------
if(_embed_assert_level_count EQUAL 0)
    set(EMBED_ASSERTION_LEVEL_CRITICAL ON)
    message(STATUS "embed: No assertion level set. Defaulting to EMBED_ASSERTION_LEVEL_CRITICAL = ON")
endif()

# Assertion behavior
# ------------------
option(EMBED_ASSERTS_AS_ASSUME "Use compiler intrinsics that asume values instead of unused asserts - for more optimisations (like __builtin_assume, or a different, or none - depending on the compiler used)" OFF)
option(EMBED_USE_EXCEPTION_CALLBACKS "Use user-defined callbacks for exceptions instead of throwing them" OFF)

# ================================================================================
#                           Output and String Formating
# ================================================================================

# ANSI Formating
# --------------
# TODO: change to: `option(EMBED_ENABLE_ANSI_CODES "Disable ANSI color escape codes in output" ON)`
option(EMBED_DISABLE_ANSI_CODES "Disable ANSI color escape codes in output" OFF)

# General Number Formating
# ------------------------
option(EMBED_FMT_MINIMAL "If ON, uses the settings from the macros instead. This disables runtime configurability but offers more performant code and smaller binaries" OFF)

# Bool Formating
# --------------
option(EMBED_FMT_BOOL_TO_TEXT "Enables bool formating as text instead of numbers. Depends on `EMBED_FMT_MINIMAL`." ON)

# Number Formating
# -----------------
# TODO: add `EMBED_ULTRA_MINIMAL_FLOAT` ... also write the implementation for it

option(EMBED_FMT_PAD_SIGN "Enables sign padding. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DOT_AS_COMMA "Use dot '.' as comma and ',' as thousands. Depends on `EMBED_FMT_MINIMAL`." ON)
option(EMBED_FMT_THOUSANDS "Enables the use of thousands. Depends on `EMBED_FMT_MINIMAL` and `EMBED_FMT_DOT_AS_COMMA`." OFF)
option(EMBED_FMT_FORCE_SIGN "forces a sign, even if the number is positive. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FORCE_COMMA "if set, makes sure that a comma is always printed for floating point numbers. Depends on `EMBED_FMT_MINIMAL`." ON)
option(EMBED_FMT_FORCE_DECIMALS "if set will make sure that the set ammount of decimals will always be printed, even if zero. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FORCE_SIGN "forces a sign, even if the number is positive. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FORCE_EXPONENT "forces an exponent, even if the exponent is zero. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FORCE_EXPONENT_SIGN "forces a sign on the exponent, even if the exponent is positive. Depends on `EMBED_FMT_MINIMAL`." OFF)

option(EMBED_FMT_DECIMALS_1 "Enables 1 decimal place. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DECIMALS_2 "Enables 2 decimal places. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DECIMALS_3 "Enables 3 decimal places. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DECIMALS_4 "Enables 4 decimal places. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DECIMALS_5 "Enables 5 decimal places. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_DECIMALS_6 "Enables 6 decimal places. Depends on `EMBED_FMT_MINIMAL`." OFF)

## Error checking of decimals 
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

## Default to EMBED_FMT_DECIMALS_3 if none set
if(_embed_fmt_decimals_count EQUAL 0)
    set(EMBED_FMT_DECIMALS_3 ON)
    message(STATUS "embed: No decimal place numer set. Defaulting to EMBED_FMT_DECIMALS_3 = ON")
endif()

## Float Representations 
option(EMBED_FMT_FLOAT_REP_SCI "Enables scientific representation. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FLOAT_REP_ENG "Enables engineering representation. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FLOAT_REP_FULL "Enables floating point representation. Depends on `EMBED_FMT_MINIMAL`." OFF)

# Check if maximal 1 of the float representations has been set
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

# Default to EMBED_FMT_FLOAT_REP_ENG if none set
if(_embed_fmt_float_rep_count EQUAL 0)
    set(EMBED_FMT_FLOAT_REP_ENG ON)
    message(STATUS "embed: No floating point representation set. Defaulting to EMBED_FMT_FLOAT_REP_ENG = ON")
endif()


# ================================================================================
#                              Summary of Options
# ================================================================================


set(embed_cmake_flags
    EMBED_COMPILE_TESTS
    EMBED_USE_EMBED_SYS_STUBS
)


set(embed_main_flags
    EMBED_DISABLE_ASSERTIONS
    EMBED_ASSERTION_LEVEL_CRITICAL
    EMBED_ASSERTION_LEVEL_O1
    EMBED_ASSERTION_LEVEL_FULL
    EMBED_ASSERTS_AS_ASSUME
    EMBED_USE_EXCEPTION_CALLBACKS
    EMBED_DISABLE_ANSI_CODES
    EMBED_FMT_MINIMAL
)

set(embed_fmt_flags
    EMBED_FMT_BOOL_TO_TEXT
    EMBED_FMT_FLOAT_REP_SCI
    EMBED_FMT_FLOAT_REP_ENG
    EMBED_FMT_FLOAT_REP_FULL
    EMBED_FMT_DECIMALS_1
    EMBED_FMT_DECIMALS_2
    EMBED_FMT_DECIMALS_3
    EMBED_FMT_DECIMALS_4
    EMBED_FMT_DECIMALS_5
    EMBED_FMT_DECIMALS_6
    EMBED_FMT_DOT_AS_COMMA
    EMBED_FMT_PAD_SIGN
    EMBED_FMT_THOUSANDS
    EMBED_FMT_FORCE_COMMA
    EMBED_FMT_FORCE_DECIMALS
    EMBED_FMT_FORCE_SIGN
    EMBED_FMT_FORCE_EXPONENT
    EMBED_FMT_FORCE_EXPONENT_SIGN
)

# Print CMake Options
message(STATUS "")
message(STATUS "embed: CMake Options:")
message(STATUS "---------------------")
foreach(var IN LISTS embed_cmake_flags)
    message(STATUS "  ${var} = ${${var}}")

    if("${var}" STREQUAL "EMBED_USE_EMBED_SYS_STUBS" AND NOT EMBED_USE_EMBED_SYS_STUBS)
        message(STATUS "    Tip: `EMBED_USE_EMBED_SYS_STUBS` and link to `embed_sys_stubs`.")
        message(STATUS "         reduces binary size for your bare-metal embedded system\n")
    endif()
endforeach()

# Print Compile Definitions
message(STATUS "")
message(STATUS "embed: Compile Definitions:")
message(STATUS "---------------------------")
foreach(var IN LISTS embed_main_flags)
    message(STATUS "  ${var} = ${${var}}")
    if("${var}" STREQUAL "EMBED_FMT_MINIMAL" AND EMBED_FMT_MINIMAL)
    message(STATUS "+ embed: Formatting Options:")
    message(STATUS "  --------------------------")
    foreach(var IN LISTS embed_fmt_flags)
        message(STATUS "    ${var} = ${${var}}")
    endforeach()
endif()
endforeach()

message(STATUS "")

# ================================================================================
#                              Apply Config
# ================================================================================

# Combine assertion + formatting flags
set(embed_all_definitions ${embed_main_flags} ${embed_fmt_flags})

foreach(var IN LISTS embed_all_definitions)
    target_compile_definitions(embed PUBLIC
        $<$<BOOL:${${var}}>:${var}>
    )
endforeach()