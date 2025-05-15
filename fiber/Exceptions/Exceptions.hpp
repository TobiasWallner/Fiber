#pragma once

//std
#include <exception>

// fiber
#include <fiber/Core/definitions.hpp>

// #include <fiber/OStream/OStream.hpp> // foreward declare instead to resolve header conflicts
namespace fiber{
    class OStream;    
} // namespace fiber;

// TODO: add a switch `FIBER_STRIP_EXCEPTIONS` that removes most of the members, diagnostics and strings to reduce binary size and overhead for strongly constrained systems

namespace fiber{

    

    /// @brief The base class for exceptions in `fiber`
    class Exception : public std::exception {
    private:
        const char* const _what = "";
        const char* const _type = "Exception";

    public:
        Exception();
        explicit Exception(const char* what) : _what(what){}

    protected:
        /// @brief Constructor for classes that derive from Exception
        /// @param type Name for the type of exception
        /// @param what Message of the exception
        Exception(const char* type, const char* what) : _what(what), _type(type){}

    public:
        constexpr const char* type() const noexcept {return this->_type;}
        const char* what() const noexcept final;
        virtual void print(fiber::OStream& stream) const;   
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

        virtual void print(OStream& stream) const final;
    };

    class AssertionFailureO1 : public AssertionFailure{
        public:

        inline AssertionFailureO1(char const* condition, char const* function_signature)
            : AssertionFailure("AssertionFailure:O1", condition, function_signature){}

        inline AssertionFailureO1(char const* condition, char const* message, char const* function_signature)     
            : AssertionFailure("AssertionFailure:O1", condition, message, function_signature){}

        virtual void print(OStream& stream) const final;
    };

    class AssertionFailureFull : public AssertionFailure{
        public:

        inline AssertionFailureFull(char const* condition, char const* function_signature)
            : AssertionFailure("AssertionFailure:FULL", condition, function_signature){}

        inline AssertionFailureFull(char const* condition, char const* message, char const* function_signature)     
            : AssertionFailure("AssertionFailure:FULL", condition, message, function_signature){}

        virtual void print(OStream& stream) const final;
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

        virtual void print(OStream& stream) const final;
    };

    #if defined(FIBER_USE_EXCEPTION_CALLBACKS)
        inline void default_exception_callback(const Exception& e){e.print(fiber::cerr); while(true){/* trap */}}
        void (*exception_callback)(const Exception& e) = default_exception_callback;
        #define FIBER_THROW(exception) exception_callback(exception); while(true){/* trap */}
    #elif defined(FIBER_DISABLE_EXCEPTIONS)
        #define FIBER_THROW(exception) ((void)sizeof(exception)); std::terminate();
    #else
        #define FIBER_THROW(exception) throw exception
    #endif

    // set the standard assertion level
    #if (defined(FIBER_ASSERTION_LEVEL_CRITICAL) + defined(FIBER_ASSERTION_LEVEL_O1) + defined(FIBER_ASSERTION_LEVEL_FULL) + defined(FIBER_DISABLE_ASSERTIONS)) > 1
        #error "Multiple assertion levels are set. Only set one: `FIBER_DISABLE_ASSERTIONS`, `FIBER_ASSERTION_LEVEL_CRITICAL`, `FIBER_ASSERTION_LEVEL_O1` or `FIBER_ASSERTION_LEVEL_FULL`."
    #elif !(defined(FIBER_ASSERTION_LEVEL_CRITICAL) || defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL) || defined(FIBER_DISABLE_ASSERTIONS))
        #pragma message("fiber: No assertion level set. Defaulting to FIBER_ASSERTION_LEVEL_CRITICAL")
        #define FIBER_ASSERTION_LEVEL_CRITICAL
    #endif

    // asserts that will have less than 5% performance hit - like checking if a container is not empty before doing some calculations
    #if (defined(FIBER_ASSERTION_LEVEL_CRITICAL) || defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL)) && !defined(FIBER_DISABLE_ASSERTIONS)
        #define FIBER_ASSERT_CRITICAL(condition) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureCritical(#condition, FIBER_FUNCTION_SIGNATURE))
        #define FIBER_ASSERT_CRITICAL_MSG(condition, message) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureCritical(#condition, message, FIBER_FUNCTION_SIGNATURE))
    #elif defined(FIBER_ASSERTS_AS_ASSUME)
        #define FIBER_ASSERT_CRITICAL(condition) FIBER_ASSUME(condition)
        #define FIBER_ASSERT_CRITICAL_MSG(condition, message) FIBER_ASSUME(condition); FIBER_USE_UNUSED(message)
    #else
        #define FIBER_ASSERT_CRITICAL(condition) FIBER_USE_UNUSED(condition)
        #define FIBER_ASSERT_CRITICAL_MSG(condition, message) FIBER_USE_UNUSED(condition); FIBER_USE_UNUSED(message)
    #endif

    // asserts that may have more than 5% performance hit but can be done in O1 time - like checking if the index on `operator[]` is in the range of the container
    #if (defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL)) && !defined(FIBER_DISABLE_ASSERTIONS)
        #define FIBER_ASSERT_O1(condition) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureO1(#condition, FIBER_FUNCTION_SIGNATURE))
        #define FIBER_ASSERT_O1_MSG(condition, message) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureO1(#condition, message, FIBER_FUNCTION_SIGNATURE))
    #elif defined(FIBER_ASSERTS_AS_ASSUME)
        #define FIBER_ASSERT_O1(condition) FIBER_ASSUME(condition)
        #define FIBER_ASSERT_O1_MSG(condition, message) FIBER_ASSUME(condition); FIBER_USE_UNUSED(message)
    #else
        #define FIBER_ASSERT_O1(condition) FIBER_USE_UNUSED(condition)
        #define FIBER_ASSERT_O1_MSG(condition, message) FIBER_USE_UNUSED(condition); FIBER_USE_UNUSED(message)
    #endif

    // asserts everything that can be asserted - the algorithm expects a sorted list to do binary search and will check if the list is really sorted
    #if (defined(FIBER_ASSERTION_LEVEL_FULL)) && !defined(FIBER_DISABLE_ASSERTIONS)
        #define FIBER_ASSERT_FULL(condition) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureFull(#condition, FIBER_FUNCTION_SIGNATURE))
        #define FIBER_ASSERT_FULL_MSG(condition, message) FIBER_IF_UNLIKELY(!(condition)) FIBER_THROW(fiber::AssertionFailureFull(#condition, message, FIBER_FUNCTION_SIGNATURE))
    #elif defined(FIBER_ASSERTS_AS_ASSUME)
        #define FIBER_ASSERT_FULL(condition) FIBER_ASSUME(condition)
        #define FIBER_ASSERT_O1_MSG(condition, message) FIBER_ASSUME(condition); FIBER_USE_UNUSED(message)
    #else
        #define FIBER_ASSERT_FULL(condition) FIBER_USE_UNUSED(condition)
        #define FIBER_ASSERT_FULL_MSG(condition, message) FIBER_USE_UNUSED(condition); FIBER_USE_UNUSED(message)
    #endif

}
