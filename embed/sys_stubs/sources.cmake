
if(EMBED_USE_EMBED_SYS_STUBS)
    add_library(embed_sys_stubs OBJECT 
        ${CMAKE_CURRENT_LIST_DIR}/sys_stubs.cpp
    )

    target_link_options(embed_sys_stubs PUBLIC
        -Wl,--wrap=atexit
    )

endif()