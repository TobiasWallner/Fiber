#pragma once

/** @file definitions.hpp
 * Useful definitions that are used for example to abstract away compiler specific macros or built in functions.
 * 
 * Note that all abstractions, that abstract compilers away always have a functioning default, 
 * so that even if your compiler is not explicitly handeled the code will still compile and run.
 */


 // compiler independent assume
#if (defined(__clang__) || defined(__GNUC__))
    #define FIBER_ASSUME(cond) __builtin_assume(cond)
#elif defined(_MSC_VER)
    #define FIBER_ASSUME(cond) __assume(cond)
#else
    #define FIBER_ASSUME(cond)
#endif

// compiler independent prettiest name to get a string of the function signature
#if defined(__clang__) || defined(__GNUC__)
  #define FIBER_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define FIBER_FUNCTION_SIGNATURE __FUNCSIG__
#else // defined(__func__)
  #define FIBER_FUNCTION_SIGNATURE __func__
#endif


#if __cplusplus >= 202002L
    // Use standard C++20 attributes
    #define FIBER_IF_LIKELY(condition)    if(condition) [[likely]]
    #define FIBER_IF_UNLIKELY(condition)  if(condition) [[unlikely]]
#elif defined(__clang__) || defined(__GNUC__)
    // Use compiler builtins
    #define FIBER_IF_LIKELY(condition)    if(__builtin_expect(condition, 1))
    #define FIBER_IF_UNLIKELY(condition)  if(__builtin_expect(condition, 0))
#else
    // No-op fallback
    #define FIBER_LIKELY                  if(condition)
    #define FIBER_UNLIKELY                if(condition)
#endif

#define FIBER_USE_UNUSED(value) ((void)sizeof(value))

#if defined(__GNUC__) || defined(__clang__)
    // Covers GCC, Clang, ARM-GCC, ARMCLANG (Keil 6)
    #define FIBER_WEAK __attribute__((weak))
#elif defined(__CC_ARM)
    // ARM Compiler 5 (Keil <=5.x), old ARMCC
    #define FIBER_WEAK __weak
#elif defined(_MSC_VER)
    // Microsoft Visual Studio (limited to inline-style weak symbols)
    #define FIBER_WEAK __declspec(selectany)
#else
    #define FIBER_WEAK
    #warning "FIBER_WEAK not defined for this compiler. Please file an issue at the repository, state the compiler you use and how one defines weak symbols for it."
#endif