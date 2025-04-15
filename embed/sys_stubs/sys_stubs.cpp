#include <stddef.h>
#include <exception>


namespace __cxxabiv1{
    std::terminate_handler __terminate_handler = +[](){ while(true){ continue; /* trap */ }};
}


// Disable Exit / Destructor Infrastructure

/**
 * @brief Wrapper for `atexit`, returns 0 without registering anything.
 * 
 * On normal systems, atexit registers a function to be called on exit. 
 * Embedded systems do not use exit(), so it is safe to skip.
 * Saves flash by eliminating destructor registration.
 * 
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=atexit
 * ```
 */
extern "C" int __wrap_atexit(void (*)(void)) { return 0; }


