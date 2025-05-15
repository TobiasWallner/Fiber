
# ================================================================================
#                                Functions for repeated tasks
# ================================================================================

function(set_exactly_one option_list default_option)
    set(_enabled_count 0)

    foreach(opt IN LISTS option_list)
        if(${opt})
            math(EXPR _enabled_count "${_enabled_count} + 1")
        endif()
    endforeach()

    if(_enabled_count GREATER 1)
        message(FATAL_ERROR
            "Multiple options are enabled!\n"
            "Please enable only ONE of the following:"
        )
        foreach(opt IN LISTS option_list)
            message(FATAL_ERROR "  - ${opt} = ${${opt}}")
        endforeach()
    endif()

    if(_enabled_count EQUAL 0)
        set(${default_option} ON CACHE BOOL "Defaulted to ON by set_exactly_one_option()" FORCE)
        message(STATUS "fiber: Defaulting to: ${default_option} = ON")
    endif()
endfunction()

function(set_default_cache_variable name default_value description)
    if(NOT DEFINED ${name})
        set(${name} "${default_value}" CACHE STRING "${description}")
        message(STATUS "fiber: Defaulting to: ${name} = ${default_value}")
    endif()
endfunction()

# ================================================================================
#                                Compile Tests
# ================================================================================

option(FIBER_CTEST "Enables testing with ctest on local host machines" OFF)
option(FIBER_COMPILE_TESTS "If ON compiles the test sources" OFF)

# ================================================================================
#                                CPU
# ================================================================================

option(FIBER_MULTI_CORE "Enables multicore features and safeguards" OFF)

# ================================================================================
#                                system stubs
# ================================================================================

option(
    FIBER_USE_FIBER_SYS_STUBS 
    "Enables system stubs that prevent the usage of unnecessary standard library features and massively reduces binary size"
    OFF)
# ================================================================================
#                                system scheduler clock
# ================================================================================

option(FIBER_CLOCK_UINT8 "Use an 8 bit integer for the clock counter" OFF)
option(FIBER_CLOCK_UINT16 "Use an 16 bit integer for the clock counter" OFF)
option(FIBER_CLOCK_UINT32 "Use an 32 bit integer for the clock counter" OFF)
option(FIBER_CLOCK_UINT64 "Use an 64 bit integer for the clock counter" OFF)

set(_fiber_clock_representations
    FIBER_CLOCK_UINT8
    FIBER_CLOCK_UINT16
    FIBER_CLOCK_UINT32
    FIBER_CLOCK_UINT64
)
set_exactly_one("${_fiber_clock_representations}" FIBER_CLOCK_UINT32)

set_default_cache_variable(FIBER_RTC_FREQ_HZ 20000 "Frequency in hertz, used for the clock tick and task scheduling")

# ================================================================================
#                                Exceptions
# ================================================================================

option(FIBER_DISABLE_ASSERTIONS "Disable all fiberOS assertions" OFF)
option(FIBER_DISABLE_EXCEPTIONS "Disables exceptions, will also disable assertions" OFF)

if(FIBER_DISABLE_EXCEPTIONS AND NOT FIBER_DISABLE_ASSERTIONS)
    message(STATUS "fiber: Disabling assertions `FIBER_DISABLE_ASSERTIONS = ON`, because `FIBER_DISABLE_EXCEPTIONS = ON`")
    set(FIBER_DISABLE_ASSERTIONS ON)
endif()


# ================================================================================
#                                Assertions
# ================================================================================

# Assertion Levels
# ----------------

option(FIBER_ASSERTION_LEVEL_CRITICAL   "Enable critical level assertions" OFF)
option(FIBER_ASSERTION_LEVEL_O1         "Enable O[1] cost development assertions + critical" OFF)
option(FIBER_ASSERTION_LEVEL_FULL       "Enable full deep validation assertions + O[1] + full" OFF)

set(_fiber_assertion_levels
    FIBER_DISABLE_ASSERTIONS
    FIBER_ASSERTION_LEVEL_CRITICAL
    FIBER_ASSERTION_LEVEL_O1
    FIBER_ASSERTION_LEVEL_FULL
)
set_exactly_one("${_fiber_assertion_levels}" FIBER_ASSERTION_LEVEL_O1)

# Assertion behavior
# ------------------
option(FIBER_ASSERTS_AS_ASSUME "Use compiler intrinsics that asume values instead of unused asserts - for more optimisations (like __builtin_assume, or a different, or none - depending on the compiler used)" OFF)
option(FIBER_USE_EXCEPTION_CALLBACKS "Use user-defined callbacks for exceptions instead of throwing them" OFF)

# ================================================================================
#                           Output and String Formating
# ================================================================================

# Encoding
# --------------
option(FIBER_DISABLE_ANSI_CODES "Enables ANSI color escape codes in output" OFF)
option(FIBER_DISABLE_UTF8_CODES "If `OFF` enables UTF8 codes, for exampample lines and borders. If `ON`, uses ASCII alternatives" OFF)

# General Number Formating
# ------------------------
option(FIBER_FMT_MINIMAL "Disables runtime configurability but offers more performant code and smaller binaries" OFF)

# Bool Formating
# --------------
option(FIBER_FMT_BOOL_TO_TEXT "Enables bool formating as text instead of numbers. Depends on `FIBER_FMT_MINIMAL`." ON)

# Number Formating
# -----------------
# TODO: add `FIBER_ULTRA_MINIMAL_FLOAT` ... also write the implementation for it

option(FIBER_FMT_PAD_SIGN "Enables sign padding. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_DOT_AS_COMMA "Use dot '.' as comma and ',' as thousands. Depends on `FIBER_FMT_MINIMAL`." ON)
option(FIBER_FMT_THOUSANDS "Enables the use of thousands. Depends on `FIBER_FMT_MINIMAL` and `FIBER_FMT_DOT_AS_COMMA`." OFF)
option(FIBER_FMT_FORCE_SIGN "forces a sign, even if the number is positive. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FORCE_COMMA "if set, makes sure that a comma is always printed for floating point numbers. Depends on `FIBER_FMT_MINIMAL`." ON)
option(FIBER_FMT_FORCE_DECIMALS "if set will make sure that the set ammount of decimals will always be printed, even if zero. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FORCE_SIGN "forces a sign, even if the number is positive. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FORCE_EXPONENT "forces an exponent, even if the exponent is zero. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FORCE_EXPONENT_SIGN "forces a sign on the exponent, even if the exponent is positive. Depends on `FIBER_FMT_MINIMAL`." OFF)

set_default_cache_variable(FIBER_FMT_FLOAT_DECIMALS 3 "Number of decimal places for float formatting. Depends on `FIBER_FMT_MINIMAL`.")


## Float Representations 
option(FIBER_FMT_FLOAT_REP_SCI "Enables scientific representation. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FLOAT_REP_ENG "Enables engineering representation. Depends on `FIBER_FMT_MINIMAL`." OFF)
option(FIBER_FMT_FLOAT_REP_FULL "Enables floating point representation. Depends on `FIBER_FMT_MINIMAL`." OFF)

## Error checking of decimals 
set(_fiber_float_representation
    FIBER_FMT_FLOAT_REP_SCI
    FIBER_FMT_FLOAT_REP_ENG
    FIBER_FMT_FLOAT_REP_FULL
)
set_exactly_one("${_fiber_float_representation}" FIBER_FMT_FLOAT_REP_ENG)

# ================================================================================
#                              Summary of Options
# ================================================================================

set(fiber_cmake_flags
    FIBER_COMPILE_TESTS
    FIBER_USE_FIBER_SYS_STUBS
)

set(fiber_main_flags
    FIBER_MULTI_CORE
    FIBER_DISABLE_EXCEPTIONS
    FIBER_DISABLE_ASSERTIONS
    FIBER_ASSERTION_LEVEL_CRITICAL
    FIBER_ASSERTION_LEVEL_O1
    FIBER_ASSERTION_LEVEL_FULL
    FIBER_ASSERTS_AS_ASSUME
    FIBER_USE_EXCEPTION_CALLBACKS
    FIBER_CLOCK_UINT8
    FIBER_CLOCK_UINT16
    FIBER_CLOCK_UINT32
    FIBER_CLOCK_UINT64
    FIBER_RTC_FREQ_HZ
    FIBER_DISABLE_ANSI_CODES
    FIBER_DISABLE_UTF8_CODES
    FIBER_FMT_MINIMAL
)

set(fiber_fmt_flags
    FIBER_FMT_BOOL_TO_TEXT
    FIBER_FMT_FLOAT_REP_SCI
    FIBER_FMT_FLOAT_REP_ENG
    FIBER_FMT_FLOAT_REP_FULL
    FIBER_FMT_DOT_AS_COMMA
    FIBER_FMT_PAD_SIGN
    FIBER_FMT_FLOAT_DECIMALS
    FIBER_FMT_THOUSANDS
    FIBER_FMT_FORCE_COMMA
    FIBER_FMT_FORCE_DECIMALS
    FIBER_FMT_FORCE_SIGN
    FIBER_FMT_FORCE_EXPONENT
    FIBER_FMT_FORCE_EXPONENT_SIGN
)

# Print CMake Options
message(STATUS "")
message(STATUS "fiber: CMake Options:")
message(STATUS "---------------------")
foreach(var IN LISTS fiber_cmake_flags)
    message(STATUS "  ${var} = ${${var}}")

    if("${var}" STREQUAL "FIBER_USE_FIBER_SYS_STUBS" AND NOT FIBER_USE_FIBER_SYS_STUBS)
        message(STATUS "    Tip: `FIBER_USE_FIBER_SYS_STUBS` and link to `fiber_sys_stubs`.")
        message(STATUS "         reduces binary size for your bare-metal embedded system\n")
    endif()
endforeach()

# Print Compile Definitions
message(STATUS "")
message(STATUS "fiber: Compile Definitions:")
message(STATUS "---------------------------")
foreach(var IN LISTS fiber_main_flags)
    message(STATUS "  ${var} = ${${var}}")
    if("${var}" STREQUAL "FIBER_FMT_MINIMAL" AND FIBER_FMT_MINIMAL)
        message(STATUS "  + Formatting Options:")
        foreach(var IN LISTS fiber_fmt_flags)
            message(STATUS "    ${var} = ${${var}}")
        endforeach()
    endif()
endforeach()

message(STATUS "")

# ================================================================================
#                              Apply Config
# ================================================================================

# Define main definitions
foreach(var IN LISTS fiber_main_flags)
    if("${${var}}" MATCHES "^-?[0-9]+$")
        # handle integers
        target_compile_definitions(fiber PUBLIC ${var}=${${var}})   
    elseif(${${var}})
        # default: define bool
        target_compile_definitions(fiber PUBLIC ${var})
    endif()
endforeach()

# Conditionally define additional formating options
if(FIBER_FMT_MINIMAL)
    foreach(var IN LISTS fiber_fmt_flags)
        if("${${var}}" MATCHES "^-?[0-9]+$")
            # handle integers
            target_compile_definitions(fiber PUBLIC ${var}=${${var}})   
        elseif(${${var}})
            # default: define bool
            target_compile_definitions(fiber PUBLIC ${var})
        endif()
    endforeach()
endif()