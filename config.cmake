include(set_exactly_one.cmake)

# ================================================================================
#                                Compile Tests
# ================================================================================

option(EMBED_CTEST "Enables testing with ctest on local host machines" OFF)
option(EMBED_COMPILE_TESTS "If ON compiles the test sources" OFF)

# ================================================================================
#                                system stubs
# ================================================================================

option(
    EMBED_USE_EMBED_SYS_STUBS 
    "Enables system stubs that prevent the usage of unnecessary standard library features and massively reduces binary size"
    OFF)
# ================================================================================
#                                system scheduler clock
# ================================================================================

option(EMBED_CLOCK_UINT8 "Use an 8 bit integer for the clock counter" OFF)
option(EMBED_CLOCK_UINT16 "Use an 16 bit integer for the clock counter" OFF)
option(EMBED_CLOCK_UINT32 "Use an 32 bit integer for the clock counter" OFF)
option(EMBED_CLOCK_UINT64 "Use an 64 bit integer for the clock counter" OFF)

set(_embed_clock_representations
    EMBED_CLOCK_UINT8
    EMBED_CLOCK_UINT16
    EMBED_CLOCK_UINT32
    EMBED_CLOCK_UINT64
)
set_exactly_one("${_embed_clock_representations}" EMBED_CLOCK_UINT32)

set_default_cache_variable(EMBED_RTC_FREQ_HZ 20000 "Frequency in hertz, used for the clock tick and task scheduling")

# ================================================================================
#                                Exceptions
# ================================================================================

option(EMBED_DISABLE_ASSERTIONS "Disable all embedOS assertions" OFF)
option(EMBED_DISABLE_EXCEPTIONS "Disables exceptions, will also disable assertions" OFF)

if(EMBED_DISABLE_EXCEPTIONS AND NOT EMBED_DISABLE_ASSERTIONS)
    message(STATUS "embed: Disabling assertions `EMBED_DISABLE_ASSERTIONS = ON`, because `EMBED_DISABLE_EXCEPTIONS = ON`")
    set(EMBED_DISABLE_ASSERTIONS ON)
endif()


# ================================================================================
#                                Assertions
# ================================================================================

# Assertion Levels
# ----------------

option(EMBED_ASSERTION_LEVEL_CRITICAL   "Enable critical level assertions" OFF)
option(EMBED_ASSERTION_LEVEL_O1         "Enable O[1] cost development assertions + critical" OFF)
option(EMBED_ASSERTION_LEVEL_FULL       "Enable full deep validation assertions + O[1] + full" OFF)

set(_embed_assertion_levels
    EMBED_DISABLE_ASSERTIONS
    EMBED_ASSERTION_LEVEL_CRITICAL
    EMBED_ASSERTION_LEVEL_O1
    EMBED_ASSERTION_LEVEL_FULL
)
set_exactly_one("${_embed_assertion_levels}" EMBED_ASSERTION_LEVEL_O1)

# Assertion behavior
# ------------------
option(EMBED_ASSERTS_AS_ASSUME "Use compiler intrinsics that asume values instead of unused asserts - for more optimisations (like __builtin_assume, or a different, or none - depending on the compiler used)" OFF)
option(EMBED_USE_EXCEPTION_CALLBACKS "Use user-defined callbacks for exceptions instead of throwing them" OFF)

# ================================================================================
#                           Output and String Formating
# ================================================================================

# Encoding
# --------------
option(EMBED_DISABLE_ANSI_CODES "Enables ANSI color escape codes in output" OFF)
option(EMBED_DISABLE_UTF8_CODES "If `OFF` enables UTF8 codes, for exampample lines and borders. If `ON`, uses ASCII alternatives" OFF)

# General Number Formating
# ------------------------
option(EMBED_FMT_MINIMAL "Disables runtime configurability but offers more performant code and smaller binaries" OFF)

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

set_default_cache_variable(EMBED_FMT_FLOAT_DECIMALS 3 "Number of decimal places for float formatting. Depends on `EMBED_FMT_MINIMAL`.")


## Float Representations 
option(EMBED_FMT_FLOAT_REP_SCI "Enables scientific representation. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FLOAT_REP_ENG "Enables engineering representation. Depends on `EMBED_FMT_MINIMAL`." OFF)
option(EMBED_FMT_FLOAT_REP_FULL "Enables floating point representation. Depends on `EMBED_FMT_MINIMAL`." OFF)

## Error checking of decimals 
set(_embed_float_representation
    EMBED_FMT_FLOAT_REP_SCI
    EMBED_FMT_FLOAT_REP_ENG
    EMBED_FMT_FLOAT_REP_FULL
)
set_exactly_one("${_embed_float_representation}" EMBED_FMT_FLOAT_REP_ENG)

# ================================================================================
#                              Summary of Options
# ================================================================================

set(embed_cmake_flags
    EMBED_COMPILE_TESTS
    EMBED_USE_EMBED_SYS_STUBS
)

set(embed_main_flags
    EMBED_DISABLE_EXCEPTIONS
    EMBED_DISABLE_ASSERTIONS
    EMBED_ASSERTION_LEVEL_CRITICAL
    EMBED_ASSERTION_LEVEL_O1
    EMBED_ASSERTION_LEVEL_FULL
    EMBED_ASSERTS_AS_ASSUME
    EMBED_USE_EXCEPTION_CALLBACKS
    EMBED_CLOCK_UINT8
    EMBED_CLOCK_UINT16
    EMBED_CLOCK_UINT32
    EMBED_CLOCK_UINT64
    EMBED_RTC_FREQ_HZ
    EMBED_DISABLE_ANSI_CODES
    EMBED_DISABLE_UTF8_CODES
    EMBED_FMT_MINIMAL
)

set(embed_fmt_flags
    EMBED_FMT_BOOL_TO_TEXT
    EMBED_FMT_FLOAT_REP_SCI
    EMBED_FMT_FLOAT_REP_ENG
    EMBED_FMT_FLOAT_REP_FULL
    EMBED_FMT_DOT_AS_COMMA
    EMBED_FMT_PAD_SIGN
    EMBED_FMT_FLOAT_DECIMALS
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
        message(STATUS "  + Formatting Options:")
        foreach(var IN LISTS embed_fmt_flags)
            message(STATUS "    ${var} = ${${var}}")
        endforeach()
    endif()
endforeach()

message(STATUS "")

# ================================================================================
#                              Apply Config
# ================================================================================

# Define main definitions
foreach(var IN LISTS embed_main_flags)
    if("${${var}}" MATCHES "^-?[0-9]+$")
        # handle integers
        target_compile_definitions(embed PUBLIC ${var}=${${var}})   
    elseif(${${var}})
        # default: define bool
        target_compile_definitions(embed PUBLIC ${var})
    endif()
endforeach()

# Conditionally define additional formating options
if(EMBED_FMT_MINIMAL)
    foreach(var IN LISTS embed_fmt_flags)
        if("${${var}}" MATCHES "^-?[0-9]+$")
            # handle integers
            target_compile_definitions(embed PUBLIC ${var}=${${var}})   
        elseif(${${var}})
            # default: define bool
            target_compile_definitions(embed PUBLIC ${var})
        endif()
    endforeach()
endif()