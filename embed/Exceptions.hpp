#pragma once


// std
#include <exception>

// embed
#include "OStream.hpp"

namespace embed{

    /**
     * @brief Base class for all exceptions in embedOS
     * 
     * 
     * All exceptions inherit from this class.
     * It provides two runtime string pointers:
     * - `type`: the static type of the exception
     * - `message`: an optional runtime message
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
     *  - `EMBED_ASSERTION_LEVEL_5P`: (default) Tests that will not use more than ~5% of the performance (Test takes less than ~5% of the remaining function). E.g.: Quick checks at the beginning of a function followed by a long calculation.
     *  - `EMBED_ASSERTION_LEVEL_O1`: Tests that will complete in O[1] time but use more than 5% of performance. E.g: range bound checks at every access of a container.
     *  - `EMBED_ASSERTION_LEVEL_FULL`: Tests that will use more than O[1] time. E.g.: checking if the list is sorted befor doing a binary search. Basically turns your project into a test suit.
     * 
     * ### Turning Assertions into Optimisations
     * 
     * You can turn unused assertions into optimisations. If your code is tested and you know for a fact that `EMBED_ASSERTION_LEVEL_O1` assertions will not fail, 
     * then you can switch to `EMBED_ASSERTION_LEVEL_5P` and also define:
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
     * - `EMBED_ASSERT_5P(exception)`: Use this for an assert that uses less than ~5% of the time compared to the rest of the function.
     * - `EMBED_ASSERTR_5P(exception, return_value)`: Use the 'R' variant if the function expects a return type.
     * 
     * - `EMBED_ASSERT_O1(exception)`: Use this if an assert can be done in O[1] time but would take a considerable amount of time compared to the rest of the function. (50% performance hit)
     * - `EMBED_ASSERTR_O1(exception, return_value)`: Use the 'R' variant if the function expects a return type.
     * 
     * - `EMBED_ASSERT_FULL(exception)`: Use if an assert will take more than O[1] time. Will take a considerable amount of time.
     * - `EMBED_ASSERTR_FULL(exception, return_value)`: Use the 'R' variant if the function expects a return type.
     * 
     * Depending on the assertion you want to check use on of the following
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
     * struct MyCustomException : public embed::Exception {
     * public:
     *     MyCustomException(const char* msg = "") : Exception("MyCustomException", msg) {}
     * };
     * ```
     */
    class Exception : private std::exception {
    private:
        const char* const _message = "";
        const char* const _type = "";

    
    public:
        constexpr Exception(const char* message = "", const char* type = "Exception") noexcept
            : _message(message) 
            , _type(type)
            {}
    
        /// @brief return a sting containing the error message 
        constexpr const char* what() const noexcept override {return this->_message;}
        
        constexpr const char* message() const noexcept override {return this->_message;}
        constexpr const char* type() const noexcept {return this->_type;}
    };

    OStream& operator<<(OStream& stream, const Exception& e) {
        return stream << "[Exception]: " << e.what();
    }

    #if defined(EMBED_USE_EXCEPTION_CALLBACKS)
        inline default_exception_callback(const Exception& e){embed::cerr << e; while(true){/* trap */}}
        void (*exception_callback)(const Exception& e) = default_exception_callback;
        #define EMBED_THROW(exception) exception_callback(exception); return;
        #define EMBED_THROWR(exception, return_value) exception_callback(exception); return return_value;

    #else
        #define EMBED_THROW(exception) throw exception;
        #define EMBED_THROWR(exception, return_value) throw exception;
    #endif

    // compiler independent assume
    #if (defined(__clang__) || defined(__GNUC__))
        #define EMBED_ASSUME(cond) __builtin_assume(cond)
    #elif defined(_MSC_VER)
        #define EMBED_ASSUME(cond) __assume(cond)
    #else
        #define EMBED_ASSUME(cond)
    #endif

    // set the standard assertion level
    #if (defined(EMBED_ASSERTION_LEVEL_5P) + defined(EMBED_ASSERTION_LEVEL_O1) + defined(EMBED_ASSERTION_LEVEL_FULL)) > 1
        # error "Multiple assertion levels are set. Please only set one."
    #elif !(defined(EMBED_ASSERTION_LEVEL_5P) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL))
        #define EMBED_ASSERTION_LEVEL_5P
    #endif

    // asserts that will have less than 5% performance hit - like checking if a container is not empty before doing some calculations
    #if (defined(EMBED_ASSERTION_LEVEL_5P) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_5P(condition, exception) if(!(condition)) EMBED_THROW(exception);
        #define EMBED_ASSERTR_5P(condition, exception, return_value) if(!(condition)) EMBED_THROW(exception); else return return_value;
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_5P(condition, exception) EMBED_ASSUME(condition)
        #define EMBED_ASSERTR_5P(condition, exception, return_value) EMBED_ASSUME(condition)
    #else
        #define EMBED_ASSERT_5P(condition, exception)
        #define EMBED_ASSERTR_5P(condition, exception, return_value) return return_value;
    #endif

    // asserts that may have more than 5% performance hit but can be done in O1 time - like checking if the index on `operator[]` is in the range of the container
    #if (defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_O1(condition, exception) if(!(condition)) EMBED_THROW(exception);
        #define EMBED_ASSERTR_O1(condition, exception, return_value) if(!(condition)) EMBED_THROW(exception); else return return_value;
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_O1(condition, exception) EMBED_ASSUME(condition)
        #define EMBED_ASSERTR_O1(condition, exception, return_value) EMBED_ASSUME(condition)
    #else
        #define EMBED_ASSERT_O1(condition, exception)
        #define EMBED_ASSERTR_O1(condition, exception, return_value) return return_value;
    #endif

    // asserts everything that can be asserted - the algorithm expects a sorted list to do binary search and will check if the list is really sorted
    #if (defined(EMBED_ASSERTION_LEVEL_FULL)) && !defined(EMBED_DISABLE_ASSERTIONS)
        #define EMBED_ASSERT_FULL(condition, exception) if(!(condition)) EMBED_THROW(exception);
        #define EMBED_ASSERTR_FULL(condition, exception, return_value) if(!(condition)) EMBED_THROW(exception); else return return_value;
    #elif defined(EMBED_ASSERTS_AS_ASSUME)
        #define EMBED_ASSERT_FULL(condition, exception) EMBED_ASSUME(condition)
        #define EMBED_ASSERTR_FULL(condition, exception, return_value) EMBED_ASSUME(condition)
    #else
        #define EMBED_ASSERT_FULL(condition, exception)
        #define EMBED_ASSERTR_FULL(condition, exception, return_value) return return_value;
    #endif

    // # Core exceptions

    /**
     * @brief Error that happen on memory allocation, because there is not enough memory.
     */
    class AllocationException : public Exception{
        public:
        constexpr AllocationException(const char* message) : Exception(message, "AllocationException"){}
    };

    /**
     * @brief Thrown when trying to dereference a pointer or iterator at an invalid location
     */
    class DereferenceException : public Exception{
        public:
        constexpr DereferenceException(const char* message) : Exception(message, "DereferenceException"){}
    };

    /**
     * @brief Throw when trying to access an element in a buffer via `.at()` or `operator[]` that is not within the size constraints of the buffer
     */
    class OutOfRangeException : public Exception{
        public:
        constexpr OutOfBoundException(const char* message) : Exception(message, "OutOfRangeAccessException"){}
    };

    /**
     * @brief When trying to push or emplace an object into an buffer that is already full
     */
    class BufferOverflowException : public Exception{
        public:
        constexpr BufferOverflowException(const char* message) : Exception(message, "BufferOverflowException"){}
    };

    /**
     * @brief When trying to pop an object from an empty buffer
     */
    class BufferUnderflowException : public Exception{
        public:
        constexpr BufferUnderflowException(const char* message) : Exception(message, "BufferUnderflowException"){}
    };

    /**
     * @brief When trying to read the value of a Future that has not been set yet. 
     */
    class FutureException : public Exception{
        public:
        constexpr FutureException(const char* message) : Exception(message, "FutureException"){}
    };

    /**
     * @brief When trying to read the value of an Optional, despit the optional not storing that value
     */
    class OptionalValueException : public Exception{
        public:
        constexpr OptionalValueException(const char* message) : Exception(message, "OptionalValueException"){}
    };


}
