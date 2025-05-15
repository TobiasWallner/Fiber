#pragma once

// std
#include <functional>

// fiber
#include <fiber/OStream/OStream.hpp>
#include <fiber/OStream/ansi.hpp>
#include <fiber/Core/definitions.hpp>
#include <fiber/OStream/ansi.hpp>


namespace fiber{

    static constexpr const char* indented_arrow     = "                      --> `";

    void print_failed_function_file_line(const char* signature, const char* file, std::size_t line);

    void TEST_FUNC_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* val_str);
    template<class T, class R>
    void TEST_FUNC_print_2(const char* test_func, const T& val, const R& result){
        fiber::cerr << indented_arrow << test_func << '(' << (val) << ")`" << fiber::newl;
        fiber::cerr << indented_arrow << result << fiber::newl << fiber::endl;
    }

    void TEST_FUNC2_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* lhs_str, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_FUNC2_print_2(const char* test_func, const T1& lhs, const T2& rhs, const R& result){
        fiber::cerr << indented_arrow << test_func << '(' << lhs << ", " << rhs << ")`" << fiber::newl;
        fiber::cerr << indented_arrow << result << '`' << fiber::newl << fiber::endl;
    }

    void TEST_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_BINARY_OPERATOR_print_2(const T1& lhs, const char* op, const T2& rhs, const R& result){
        fiber::cerr << indented_arrow << lhs << ' ' << op << ' ' << rhs << '`' << fiber::newl;
        fiber::cerr << indented_arrow << result << '`' << fiber::newl << fiber::endl;
    }

    void TEST_NOT_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_NOT_BINARY_OPERATOR_print_2(const T1& lhs, const char* op, const T2& rhs, const R& result){
        fiber::cerr << indented_arrow << lhs << ' ' << op << ' ' << rhs << '`' << fiber::newl;
        fiber::cerr << indented_arrow << result << '`' << fiber::newl << fiber::endl;
    }

    void TEST_TRUE_print_1(const char* signature, const char* file, std::size_t line, const char* val);
    template<class R>
    void TEST_TRUE_print_2(const R& val){
        fiber::cerr << indented_arrow << val << '`' << fiber::newl << fiber::endl;
    }

    void TEST_FALSE_print_1(const char* signature, const char* file, std::size_t line, const char* val);
    template<class R>
    void TEST_FALSE_print_2(const R& val){
        TEST_TRUE_print_2(val);
    }

    void TEST_ERROR_print(const char* signature, const char* file, std::size_t line);
    
    struct TestResult{
        enum class Type{Pass, Fail};
        size_t passed = 0;
        size_t failed = 0;

        constexpr TestResult() = default;

        constexpr TestResult(Type type) 
            : passed((type == Type::Pass) ? 1 : 0)
            , failed((type == Type::Fail) ? 1 : 0){}

        constexpr TestResult(size_t passed, size_t failed)
            : passed(passed)
            , failed(failed){}
    };
    
    constexpr TestResult operator | (TestResult lhs, TestResult rhs){
        return TestResult(lhs.passed + rhs.passed, lhs.failed + rhs.failed);
    }
    
    constexpr int operator | (TestResult lhs, int (*f)(TestResult)){
        return f(lhs);
    }

    constexpr TestResult operator | (TestResult lhs, TestResult (*test)(void)){
        return lhs | test();
    }
    
    int evaluate(TestResult result);

}// namespace fiber

#define TEST_START \
    fiber::TestResult::Type _fiber_test_result_ = fiber::TestResult::Type::Pass;

#define TEST_GROUP \
    fiber::cout << __func__ << ':' << fiber::newl;

#define TEST_END \
    fiber::cout << "    "; \
    if(_fiber_test_result_ == fiber::TestResult::Type::Pass){ \
        fiber::cout << "[" << fiber::ansi::bright_green << fiber::ansi::bold << "PASS" << fiber::ansi::reset << "]: "; \
    }else{ \
        fiber::cout << "[" << fiber::ansi::bright_red << fiber::ansi::bold << "FAIL" << fiber::ansi::reset << "]: "; \
    } \
    fiber::cout << __func__ << fiber::endl; \
    return fiber::TestResult(_fiber_test_result_);

// TODO: write nicer error messaging for the function tests
#define TEST_FUNC(function, val) { \
    if(!(function(val))){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_FUNC_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #val); \
        fiber::TEST_FUNC_print_2(#function, val, function(val)) \
    } \
}

#define TEST_FUNC2(function, lhs, rhs) { \
    if(!(function((lhs), (rhs)))){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_FUNC2_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #lhs, #rhs); \
        fiber::TEST_FUNC2_print_2(#function, (lhs), (rhs), (function((lhs), (rhs)))); \
    } \
}

#define TEST_BINARY_OPERATOR(lhs, op, rhs){ \
    if(!((lhs) op (rhs))){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_BINARY_OPERATOR_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs); \
        fiber::TEST_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs))); \
    } \
}

#define TEST_NOT_BINARY_OPERATOR(lhs, op, rhs){ \
    if(((lhs) op (rhs))){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_NOT_BINARY_OPERATOR_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs); \
        fiber::TEST_NOT_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs))); \
    } \
}

#define TEST_EQUAL(lhs, rhs)            TEST_BINARY_OPERATOR(lhs, ==, rhs)
#define TEST_NOT_EQUAL(lhs, rhs)        TEST_BINARY_OPERATOR(lhs, !=, rhs)
#define TEST_SMALLER(lhs, rhs)          TEST_BINARY_OPERATOR(lhs, <, rhs)
#define TEST_SMALLER_EQUAL(lhs, rhs)    TEST_BINARY_OPERATOR(lhs, <=, rhs)
#define TEST_GREATER(lhs, rhs)          TEST_BINARY_OPERATOR(lhs, >, rhs)
#define TEST_GREATER_EQUAL(lhs, rhs)    TEST_BINARY_OPERATOR(lhs, >=, rhs)

#define TEST_NOT_SMALLER(lhs, rhs)          TEST_NOT_BINARY_OPERATOR(lhs, <, rhs)
#define TEST_NOT_SMALLER_EQUAL(lhs, rhs)    TEST_NOT_BINARY_OPERATOR(lhs, <=, rhs)
#define TEST_NOT_GREATER(lhs, rhs)          TEST_NOT_BINARY_OPERATOR(lhs, >, rhs)
#define TEST_NOT_GREATER_EQUAL(lhs, rhs)    TEST_NOT_BINARY_OPERATOR(lhs, >=, rhs)

#define TEST_TRUE(val) { \
    if(!(val)){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_TRUE_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val); \
        fiber::TEST_TRUE_print_2(val); \
    } \
}

#define TEST_FALSE(val) { \
    if(val){ \
        _fiber_test_result_ = fiber::TestResult::Type::Fail; \
        fiber::TEST_FALSE_print_1(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val); \
        fiber::TEST_FALSE_print_2(val); \
    } \
}

#define TEST_ERROR \
    _fiber_test_result_ = fiber::TestResult::Type::Fail; \
    fiber::TEST_ERROR_print(FIBER_FUNCTION_SIGNATURE, __FILE__, __LINE__);

#ifndef FIBER_DISABLE_EXCEPTIONS
    #define TEST_THROW(callable) try{callable; TEST_ERROR;}catch(...){}
#else
    #define TEST_THROW(callable) ;
#endif


