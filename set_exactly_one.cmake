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
        message(STATUS "embed: Defaulting to: ${default_option} = ON")
    endif()
endfunction()

function(set_default_cache_variable name default_value description)
    if(NOT DEFINED ${name})
        set(${name} "${default_value}" CACHE STRING "${description}")
        message(STATUS "embed: Defaulting to: ${name} = ${default_value}")
    endif()
endfunction()