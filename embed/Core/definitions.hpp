#pragma once

/** @file definitions.hpp
 * Useful definitions that are used for example to abstract away compiler specific macros or built in functions.
 * 
 * Note that all abstractions, that abstract compilers away always have a functioning default, 
 * so that even if your compiler is not explicitly handeled the code will still compile and run.
 */


 // compiler independent assume
#if (defined(__clang__) || defined(__GNUC__))
    #define EMBED_ASSUME(cond) __builtin_assume(cond)
#elif defined(_MSC_VER)
    #define EMBED_ASSUME(cond) __assume(cond)
#else
    #define EMBED_ASSUME(cond)
#endif

// compiler independent prettiest name to get a string of the function signature
#if defined(__clang__) || defined(__GNUC__)
  #define EMBED_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define EMBED_FUNCTION_SIGNATURE __FUNCSIG__
#else // defined(__func__)
  #define EMBED_FUNCTION_SIGNATURE __func__
#endif


#if __cplusplus >= 202002L
    // Use standard C++20 attributes
    #define EMBED_IF_LIKELY(condition)    if(condition) [[likely]]
    #define EMBED_IF_UNLIKELY(condition)  if(condition) [[unlikely]]
#elif defined(__clang__) || defined(__GNUC__)
    // Use compiler builtins
    #define EMBED_IF_LIKELY(condition)    if(__builtin_expect(condition, 1))
    #define EMBED_IF_UNLIKELY(condition)  if(__builtin_expect(condition, 0))
#else
    // No-op fallback
    #define EMBED_LIKELY                  if(condition)
    #define EMBED_UNLIKELY                if(condition)
#endif

#define EMBED_USE_UNUSED(value) ((void)sizeof(value))