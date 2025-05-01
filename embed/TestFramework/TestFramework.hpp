#pragma once

// std
#include <functional>

// embed
#include <embed/OStream/OStream.hpp>
#include <embed/OStream/ansi.hpp>
#include <embed/Core/definitions.hpp>
#include <embed/OStream/ansi.hpp>


namespace embed{

    static constexpr const char* indented_arrow     = "                      --> `";

    void print_failed_function_file_line(const char* signature, const char* file, std::size_t line);

    void TEST_FUNC_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* val_str);
    template<class T, class R>
    void TEST_FUNC_print_2(const char* test_func, const T& val, const R& result){
        embed::cerr << indented_arrow << test_func << '(' << (val) << ")`" << embed::newl;
        embed::cerr << indented_arrow << result << embed::newl << embed::endl;
    }

    void TEST_FUNC2_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* lhs_str, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_FUNC2_print_2(const char* test_func, const T1& lhs, const T2& rhs, const R& result){
        embed::cerr << indented_arrow << test_func << '(' << lhs << ", " << rhs << ")`" << embed::newl;
        embed::cerr << indented_arrow << result << '`' << embed::newl << embed::endl;
    }

    void TEST_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_BINARY_OPERATOR_print_2(const T1& lhs, const char* op, const T2& rhs, const R& result){
        embed::cerr << indented_arrow << lhs << ' ' << op << ' ' << rhs << '`' << embed::newl;
        embed::cerr << indented_arrow << result << '`' << embed::newl << embed::endl;
    }

    void TEST_NOT_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str);
    template<class T1, class T2, class R>
    void TEST_NOT_BINARY_OPERATOR_print_2(const T1& lhs, const char* op, const T2& rhs, const R& result){
        embed::cerr << indented_arrow << lhs << ' ' << op << ' ' << rhs << '`' << embed::newl;
        embed::cerr << indented_arrow << result << '`' << embed::newl << embed::endl;
    }

    void TEST_TRUE_print_1(const char* signature, const char* file, std::size_t line, const char* val);
    template<class R>
    void TEST_TRUE_print_2(const R& val){
        embed::cerr << indented_arrow << val << '`' << embed::newl << embed::endl;
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

}// namespace embed

#define TEST_START \
    embed::TestResult::Type _embed_test_result_ = embed::TestResult::Type::Pass;

#define TEST_GROUP \
    embed::cout << __func__ << ':' << embed::newl;

#define TEST_END \
    embed::cout << "    "; \
    if(_embed_test_result_ == embed::TestResult::Type::Pass){ \
        embed::cout << "[" << embed::ansi::bright_green << embed::ansi::bold << "PASS" << embed::ansi::reset << "]: "; \
    }else{ \
        embed::cout << "[" << embed::ansi::bright_red << embed::ansi::bold << "FAIL" << embed::ansi::reset << "]: "; \
    } \
    embed::cout << __func__ << embed::endl; \
    return embed::TestResult(_embed_test_result_);

// TODO: write nicer error messaging for the function tests
#define TEST_FUNC(function, val) { \
    if(!(function(val))){ \
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_FUNC_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #val); \
        embed::TEST_FUNC_print_2(#function, val, function(val)) \
    } \
}

#define TEST_FUNC2(function, lhs, rhs) { \
    if(!(function((lhs), (rhs)))){ \
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_FUNC2_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #lhs, #rhs); \
        embed::TEST_FUNC2_print_2(#function, (lhs), (rhs), (function((lhs), (rhs)))); \
    } \
}

#define TEST_BINARY_OPERATOR(lhs, op, rhs){ \
    if(!((lhs) op (rhs))){ \
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_BINARY_OPERATOR_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs); \
        embed::TEST_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs))); \
    } \
}

#define TEST_NOT_BINARY_OPERATOR(lhs, op, rhs){ \
    if(((lhs) op (rhs))){ \
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_NOT_BINARY_OPERATOR_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs); \
        embed::TEST_NOT_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs))); \
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
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_TRUE_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val); \
        embed::TEST_TRUE_print_2(val); \
    } \
}

#define TEST_FALSE(val) { \
    if(val){ \
        _embed_test_result_ = embed::TestResult::Type::Fail; \
        embed::TEST_FALSE_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val); \
        embed::TEST_FALSE_print_2(val); \
    } \
}

#define TEST_ERROR \
    _embed_test_result_ = embed::TestResult::Type::Fail; \
    embed::TEST_ERROR_print(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__);

#ifndef EMBED_DISABLE_EXCEPTIONS
    #define TEST_THROW(callable) try{callable; TEST_ERROR;}catch(...){}
#else
    #define TEST_THROW(callable) ;
#endif


