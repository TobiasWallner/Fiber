
if(FIBER_USE_FIBER_SYS_STUBS)
    add_library(fiber_sys_stubs OBJECT 
        ${CMAKE_CURRENT_LIST_DIR}/sys_stubs.cpp
    )

    target_link_options(fiber_sys_stubs PUBLIC
        -Wl,--wrap=atexit
    )

endif()