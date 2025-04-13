#include <stddef.h>
#include <exception>

namespace __cxxabiv1{
    std::terminate_handler __terminate_handler = +[](){
        while(true){ 
            continue; // trap 
            }
        };
}

// Remove Stream reading writeing

struct _reent;

// Remove calls to heap memory

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

/**
 * @brief Wrapper for `__cxa_atexit`, returns 0 without registering destructors.
 * 
 * The original __cxa_atexit function is responsible for registering destructors 
 * of static and global objects, which are called during program termination. 
 * In embedded contexts with no exit, destructors are not needed. This safely disables them.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=__cxa_atexit
 * ```
 */
extern "C" int __wrap___cxa_atexit(void (*)(void*), void*, void*) { return 0; }

/**
 * @brief Wrapper for `_register_exitproc`, returns 0.
 * 
 * The original _register_exitproc function is used internally by `atexit` and `__cxa_atexit` 
 * to manage a table of exit handlers (functions to call during program termination). 
 * In embedded systems that never exit, this registration is unnecessary. 
 * Avoiding it prevents linking exit logic and reduces binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=_register_exitproc
 * ```
 */
extern "C" int __wrap__register_exitproc(int, void*, void*, void*) { return 0; }

/**
 * @brief Wrapper for `abort`, replaced with trap.
 * 
 * The original `abort` function typically prints an error message to stderr and terminates the 
 * program, often using file I/O and finalization logic. In embedded systems, this is 
 * unnecessary and often undesirable. This wrapper replaces `abort` with an intentional trap to 
 * halt execution immediately and cleanly, without pulling in libc formatting or shutdown logic
 * and reduces binary size.
 *
 * Linker flag:
 * ```
 * -Wl,--wrap=abort
 * ```
 */
extern "C" void __wrap_abort(void) { while(1){/* trap */}; }

// Remove FILE/stdio symbols

/**
 * @brief Wrapper for `fputc`, replaced with `return -1`.
 * 
 * The original `fputc` function writes a single character to a file stream, 
 * typically used for printing to `stdout` or `stderr`. In embedded systems where file 
 * streams are not used, replacing it avoids linking in unnecessary `stdio` logic and
 * reduces binary size.
 * This wrapper disables it safely, returning -1 to signal failure.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=fputc
 * ```
 */
extern "C" int __wrap_fputc(int, void*) { return -1; }

/**
 * @brief Wrapper for `fputs`, replaced with `return -1`.
 * 
 * The original `fputs` function writes a string to a file stream, such as `stdout` or `stderr`. 
 * In a full operating system, this is commonly used for diagnostic or logging output. 
 * In embedded systems where no file streams or terminals exist, it is safe to replace this 
 * with a stub. Doing so prevents output formatting logic from being linked in from the C standard 
 * library, reducing flash usage and binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=fputs
 * ```
 */
extern "C" int __wrap_fputs(const char*, void*) { return -1; }

/**
 * @brief Wrapper for `fflush`, replaced with `return 0`.
 * 
 * The original `fflush` flushes buffered output from a file stream to the target device or file, 
 * ensuring that pending output is fully written. In bare-metal embedded systems, 
 * output is usually tied to hardware abstraction layers, making flushing to files unnecessary.
 *  
 * This wrapper replaces it with a no-op returning 0. It's safe in embedded contexts and helps 
 * avoid linking libc buffering code, saving several kilobytes of flash.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=fflush
 * ```
 */
extern "C" int __wrap_fflush(void*) { return 0; }

/**
 * @brief Wrapper for `fclose`, replaced with `return 0`.
 * 
 * The original `fclose` function closes a file stream and flushes any associated buffers before 
 * releasing system resources. Since embedded systems typically do not have a file system or 
 * open file descriptors, this behavior is unnecessary. The wrapper safely stubs the function 
 * to return success without doing anything. This prevents linking in file stream handling 
 * code, reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=fclose
 * ```
 */
extern "C" int __wrap_fclose(void*) { return 0; }

/**
 * @brief Wrapper for `fwrite`, replaced with `return 0`.
 * 
 * The original `fwrite` function writes binary data to a file stream, typically used for 
 * buffered output to files or devices. In embedded systems, where standard file I/O is 
 * unavailable or replaced by lightweight streams, this operation is unnecessary. 
 * This wrapper disables it entirely, returning 0 to indicate no data written. 
 * It is safe in systems not using `FILE*`, and prevents libc `stdio` buffering code from 
 * being linked, reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=fwrite
 * ```
 */
extern "C" int __wrap_fwrite(const void*, size_t, size_t, void*) { return 0; }

/**
 * @brief Wrapper for `_fflush_r`, replaced with `return 0`.
 * 
 * The original `_fflush_r` function is a reentrant version of `fflush`, used to flush the 
 * output buffer of a stream in thread-safe or multitasking environments. 
 * In embedded systems where no buffering is used or output is unbuffered and threads are not available
 * on a single core machine, this function is unnecessary. This wrapper replaces it with a no-op returning 0, 
 * safely bypassing libc's stdio glue and reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=_fflush_r
 * ```
 */
extern "C" int __wrap__fflush_r(void*, void*) { return 0; }

/**
 * @brief Wrapper for `_fwrite_r`, replaced with `return 0`.
 * 
 * The original `_fwrite_r` function is a reentrant-safe version of `fwrite`, used to write data 
 * to a file stream in systems that support multitasking. In embedded environments with no 
 * standard I/O or file system, it is safe to stub out this function. 
 * This wrapper replaces it with a no-op returning 0, helping reduce flash usage by excluding 
 * file I/O logic and stdio dependencies.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=_fwrite_r
 * ```
 */
extern "C" int __wrap__fwrite_r(void*, const void*, size_t, size_t, void*) { return 0; }

// Stub out global object cleanup

/**
 * @brief Wrapper for `__cxa_finalize`, replaced with an empty body.
 * 
 * The original `__cxa_finalize` function is responsible for calling destructors for objects 
 * registered with `__cxa_atexit`, typically during program termination. Since embedded 
 * systems do not exit normally, this logic is unnecessary. Wrapping this function avoids 
 * linking the finalization infrastructure, reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=__cxa_finalize
 * ```
 */
extern "C" void __wrap___cxa_finalize(void*) {}

// Override weak stdlib glue

/**
 * @brief Wrapper for `__sfp_lock_acquire`, replaced with and empty body.
 * 
 * The original `__sfp_lock_acquire` function is part of newlib's internal locking system for 
 * stdio streams, used to ensure thread safety during buffered I/O. In single-threaded or 
 * bare-metal embedded environments without preemptive multitasking or `FILE*` usage, these 
 * locks are unnecessary. This wrapper disables the lock acquisition logic completely, 
 * allowing libc to avoid linking thread-safety glue, which reduces binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=__sfp_lock_acquire
 * ```
 */
extern "C" void __wrap___sfp_lock_acquire(void) {}

/**
 * @brief Wrapper for `__sfp_lock_release`, replaced with and empty body.
 * 
 * The original `__sfp_lock_release` function complements `__sfp_lock_acquire` and is used to 
 * release internal `stdio` stream locks in newlib's thread-safe implementation. 
 * In bare-metal embedded systems that do not use multithreaded `stdio` or any `FILE*` 
 * operations, this is unnecessary. Wrapping it as an empty function is safe and helps avoid 
 * linking in unnecessary synchronization logic, reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=__sfp_lock_release
 * ```
 */
extern "C" void __wrap___sfp_lock_release(void) {}

/**
 * @brief Wrapper for `__sinit`, replaced with `return NULL`.
 * 
 * The original `__sinit` function is part of newlib's internal initialization for managing 
 * `FILE*` streams. It sets up the internal structures required for `stdio` functions like 
 * `fopen`, `fwrite`, or `fclose`. In embedded systems that do not use standard file streams 
 * or `FILE*`-based I/O, this initialization is unnecessary. 
 * Wrapping this function with a stub that returns `NULL` safely disables `stdio` 
 * initialization and avoids pulling in `stdio` infrastructure, saving binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=__sinit
 * ```
 */
extern "C" void* __wrap___sinit(void*) { return NULL; }

/**
 * @brief Wrapper for `_cleanup`, replaced with an empty body.
 * 
 * The original `_cleanup` function is typically used by newlib to flush open file streams 
 * and release runtime resources during program termination. In embedded systems, where no 
 * file streams are open and the application never exits, this function is unnecessary. 
 * This wrapper safely disables runtime clean-up, avoiding the need for libc shutdown logic 
 * and reducing binary size.
 * 
 * Linker flag:
 * ```
 * -Wl,--wrap=_cleanup
 * ```
 */
extern "C" void __wrap__cleanup(void) {}

