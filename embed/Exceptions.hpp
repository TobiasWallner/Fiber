#pragma once

//std
#include <exception>

// embed
#include "embed/definitions.hpp"

// #include "embed/OStream.hpp" // foreward declare instead
namespace embed{
    class OStream;    
} // namespace embed;

// TODO: add a switch `EMBED_STRIP_EXCEPTIONS` that removes most of the members, diagnostics and strings to reduce binary size and overhead for strongly constrained systems

namespace embed{

    /**
     * @page Exceptions and Assertions
     * 
     * embed uses `embed::Exception`, which is the base class for all exceptions in embedOS.
     * This design choice has been made to allow multiple different raw-strings to be passed to exceptions
     * to generate error messages without heap memory allocation.
     *
     * @section Exception Philosophy
     *
     * Exceptions in embedOS are used **only** to signal programmer-caused setup errors,
     * such as invalid arguments, null pointers, or memory exhaustion. They are never
     * used for runtime control flow, task switching, or peripheral errors during ISR.
     *
     * This design ensures deterministic behavior and simplifies safety analysis.
     * 
     * Further no exceptions allocate memory or use the heap.
     * 
     * @section Exception Customisation
     * 
     * ## Assertion Levels
     * 
     * Assertions will - per default - throw exceptions if they fail.
     * 
     * Assertions can be customised based on the expected performance hit. For that `embed` suports 4 different levels.
     * The user can just add the corresponding define option their compilation.
     * 
     *  - `EMBED_DISABLE_ASSERTIONS`: Disables all assertions
     *  - `EMBED_ASSERTION_LEVEL_CRITICAL`: (default) Tests that will not use more than ~5% of the performance and are difficult to reason about, like memory allocation errors.
     *  - `EMBED_ASSERTION_LEVEL_O1`: Tests that will complete in O[1] time but use more than 5% of performance. E.g: range bound checks at every access of a container.
     *  - `EMBED_ASSERTION_LEVEL_FULL`: Tests that will use more than O[1] time. E.g.: checking if the list is sorted befor doing a binary search. Basically turns your project into a test suit.
     * 
     * ### Turning Assertions into Optimisations
     * 
     * You can turn unused assertions into optimisations. If your code is tested and you know for a fact that `EMBED_ASSERTION_LEVEL_O1` assertions will not fail, 
     * then you can switch to `EMBED_ASSERTION_LEVEL_CRITICAL` and also define:
     * ```
     * EMBED_ASSERTS_AS_ASSUME
     * ```
     * This will turn all unused assertions (higher layer or all if disabled) into assume statements for the compiler that can be used to further optimize the code. 
     * 
     * #### But Be Careful!
     * 
     *  If the assertion is not actually `true` at runtime, this may result in undefined behavior
     *  
     * ### How to use Assertion Levels
     * 
     * - `EMBED_ASSERT_CRITICAL(condition == true)`: Use this for an assert that uses less than ~5% of the time compared to the rest of the function.
     * - `EMBED_ASSERT_O1(condition == true)`: Use this if an assert can be done in O[1] time but would take a considerable amount of time compared to the rest of the function. (50% performance hit)
     * - `EMBED_ASSERT_FULL(condition == true)`: Use if an assert will take more than O[1] time. Will take a considerable amount of time.
     * 
     * *note that if exceptions are disabled the assertions will trap execution*
     * 
     * 
     * ## Redirect assertions from exceptions to callbacks
     * 
     * If 
     *  - you hate exceptions go watch this talk: https://www.youtube.com/watch?v=bY2FlayomlE
     *  - you still hate exceptions or your manager does and forbids you to use them
     * 
     * you can set the following definition:
     * ```
     * EMBED_USE_EXCEPTION_CALLBACKS
     * ```
     * which will make all assertions call the function-pointer
     * ```
     * embed::exception_callback(const Exception& e)
     * ```
     * instead of throwing the exception. The user can overload this function pointer with his own implementation.
     * The default implementation of `exception_callback` will print the exception to `embed::cerr` and then trapps in a while loop.
     * 
     * 
     * ## Disable all checks and assertions
     * 
     * If you want no error checking and are 100% sure that nothing will go wrong and you need that extra bit of performance,
     * then you can define the following option in your compilation to disable and remove all exceptions
     * 
     * @section Defining Custom Exceptions
     *
     * You can define your own exception like this:
     * ```cpp
     * class MyException : public Exception {
     * public:
     *     constexpr MyException(...) : ... {...}
     *     void print(OStream& stream) override {stream << "[MyException] " << ... << embed::endl;}
     * };
     * ```
     */

    /// @brief The base class for exceptions in `embed`
    class Exception : public std::exception {
    private:
        const char* const _what = "";
        const char* const _type = "Exception";

    public:
        Exception();
        Exception(const char* what) : _what(what){}

    protected:
        /// @brief Constructor for classes that derive from Exception
        /// @param type Name for the type of exception
        /// @param what Message of the exception
        Exception(const char* type, const char* what) : _what(what), _type(type){}

    public:
        constexpr const char* type() const noexcept {return this->_type;}
        const char* what() const noexcept override;
        virtual void print(embed::OStream& stream) const;   
    };

    OStream& operator<<(OStream& stream, const Exception& e);
    
    class AssertionFailure : public Exception {
    public:
        const char* const condition = nullptr;
        const char* const function_signature = nullptr;

        inline AssertionFailure(const char* type_i, char const* condition_i, char const* function_signature_i)
            : Exception(type_i, condition_i)
            , condition(nullptr)
            , function_signature(function_signature_i){}

        inline AssertionFailure(const char* type_i, char const* condition_i, char const* message_i, char const* function_signature_i)
            : Exception(type_i, message_i)
            , condition(condition_i)
            , function_signature(function_signature_i){}

        virtual void print(OStream& stream) const override;
    };

    class AssertionFailureCritical : public AssertionFailure{
        public:

        inline AssertionFailureCritical(char const* condition, char const* function_signature)
            : AssertionFailure("AssertionFailure:CRITICAL", condition, function_signature){}

        inline AssertionFailureCritical(char const* condition, char const* message, char const* function_signature)     
            : AssertionFailure("AssertionFailure:CRITICAL", condition, message, function_signature){}

        virtual void print(OStream& stream) const override;
    };

    class AssertionFailureO1 : public AssertionFailure{
        public:

        inline AssertionFailureO1(char const* condition, char const* function_signature)
            : AssertionFailure("AssertionFailure:O1", condition, function_signature){}

        inline AssertionFailureO1(char const* condition, char const* message, char const* function_signature)     
            : AssertionFailure("AssertionFailure:O1", condition, message, function_signature){}

        virtual void print(OStream& stream) const override;
    };

    class AssertionFailureFull : public AssertionFailure{
        public:

        inline AssertionFailureFull(char const* condition, char const* function_signature)
            : AssertionFailure("AssertionFailure:FULL", condition, function_signature){}

        inline AssertionFailureFull(char const* condition, char const* message, char const* function_signature)     
            : AssertionFailure("AssertionFailure:FULL", condition, message, function_signature){}

        virtual void print(OStream& stream) const override;
    };

    class AllocationFailure : public Exception {
        public:
        const std::size_t to_allocate;
        const std::size_t buffer_size;
        const std::size_t largest_free;
        const std::size_t nfree;
        const std::size_t nalloc;
        /*
        [AllocationFailure]: Failed to allocate xx bytes
        */

        inline AllocationFailure(std::size_t to_allocate, std::size_t buffer_size, std::size_t largest_free, std::size_t nfree, std::size_t nalloc) 
            : Exception("AllocationFailure", "Could not allocate memory")
            , to_allocate(to_allocate)
            , buffer_size(buffer_size)
            , largest_free(largest_free)
            , nfree(nfree)
            , nalloc(nalloc){}

        virtual void print(OStream& stream) const override;
    };

    #if defined(EMBED_USE_EXCEPTION_CALLBACKS)
        inline void default_exception_callback(const Exception& e){e.print(embed::cerr); while(true){/* trap */}}
        void (*exception_callback)(const Exception& e) = default_exception_callback;
        #define EMBED_THROW(exception) exception_callback(exception); while(true){/* trap */}
    #else
        #define EMBED_THROW(exception) throw exception
    #endif

    // set the standard assertion level
    #if (defined(EMBED_ASSERTION_LEVEL_CRITICAL) + defined(EMBED_ASSERTION_LEVEL_O1) + defined(EMBED_ASSERTION_LEVEL_FULL) + defined(EMBED_DISABLE_ASSERTIONS)) > 1
        #error "Multiple assertion levels are set. Only set one: `EMBED_DISABLE_ASSERTIONS`, `EMBED_ASSERTION_LEVEL_CRITICAL`, `EMBED_ASSERTION_LEVEL_O1` or `EMBED_ASSERTION_LEVEL_FULL`."
    #elif !(defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL) || defined(EMBED_DISABLE_ASSERTIONS))
        #pragma message("embed: No assertion level set. Defaulting to EMBED_ASSERTION_LEVEL_CRITICAL")
        #define EMBED_ASSERTION_LEVEL_CRITICAL
    #endif

    // asserts that will have less than 5% performance hit - like checking if a container is not empty before doing some calculations
    #if (defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_CRITICAL(condition) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureCritical(#condition, EMBED_FUNCTION_SIGNATURE))
        #define EMBED_ASSERT_CRITICAL_MSG(condition, message) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureCritical(#condition, message, EMBED_FUNCTION_SIGNATURE))
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_CRITICAL(condition) EMBED_ASSUME(condition)
        #define EMBED_ASSERT_CRITICAL_MSG(condition, message) EMBED_ASSUME(condition); EMBED_USE_UNUSED(message)
    #else
        #define EMBED_ASSERT_CRITICAL(condition) EMBED_USE_UNUSED(condition)
        #define EMBED_ASSERT_CRITICAL_MSG(condition, message) EMBED_USE_UNUSED(condition); EMBED_USE_UNUSED(message)
    #endif

    // asserts that may have more than 5% performance hit but can be done in O1 time - like checking if the index on `operator[]` is in the range of the container
    #if (defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_O1(condition) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureO1(#condition, EMBED_FUNCTION_SIGNATURE))
        #define EMBED_ASSERT_O1_MSG(condition, message) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureO1(#condition, message, EMBED_FUNCTION_SIGNATURE))
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_O1(condition) EMBED_ASSUME(condition)
        #define EMBED_ASSERT_O1_MSG(condition, message) EMBED_ASSUME(condition); EMBED_USE_UNUSED(message)
    #else
        #define EMBED_ASSERT_O1(condition) EMBED_USE_UNUSED(condition)
        #define EMBED_ASSERT_O1_MSG(condition, message) EMBED_USE_UNUSED(condition); EMBED_USE_UNUSED(message)
    #endif

    // asserts everything that can be asserted - the algorithm expects a sorted list to do binary search and will check if the list is really sorted
    #if (defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_FULL(condition) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureFull(#condition, EMBED_FUNCTION_SIGNATURE))
        #define EMBED_ASSERT_FULL_MSG(condition, message) EMBED_IF_UNLIKELY(!(condition)) EMBED_THROW(embed::AssertionFailureFull(#condition, message, EMBED_FUNCTION_SIGNATURE))
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_FULL(condition) EMBED_ASSUME(condition)
        #define EMBED_ASSERT_O1_MSG(condition, message) EMBED_ASSUME(condition); EMBED_USE_UNUSED(message)
    #else
        #define EMBED_ASSERT_FULL(condition) EMBED_USE_UNUSED(condition)
        #define EMBED_ASSERT_FULL_MSG(condition, message) EMBED_USE_UNUSED(condition); EMBED_USE_UNUSED(message)
    #endif


    // -----------------------------------------------------------------------------------------------------------------------------------------------------

    
    #if (defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_THROW_CRITICAL(exception) EMBED_THROW(exception)
    #else
        #define EMBED_THROW_CRITICAL(exception) ((void)0)
    #endif

}
