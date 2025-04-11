#pragma once

// std
#include <functional>

// embed
#include "OStream.hpp"
#include "definitions.hpp"
#include "ansi.hpp"


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
    
}

// TODO: write nicer error messaging for the function tests
#define TEST_FUNC(function, val) {\
    if(!(function(val))){\
        embed::TEST_FUNC_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #val); \
        embed::TEST_FUNC_print_2(#function, val, function(val)) \
    }\
}

#define TEST_FUNC2(function, lhs, rhs) {\
    if(!(function((lhs), (rhs)))){\
        embed::TEST_FUNC2_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #function, #lhs, #rhs);\
        embed::TEST_FUNC2_print_2(#function, (lhs), (rhs), (function((lhs), (rhs))));\
    }\
}

#define TEST_BINARY_OPERATOR(lhs, op, rhs){\
    if(!((lhs) op (rhs))){\
        embed::TEST_BINARY_OPERATOR_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs);\
        embed::TEST_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs)));\
    }\
}

#define TEST_NOT_BINARY_OPERATOR(lhs, op, rhs){\
    if(((lhs) op (rhs))){\
        embed::TEST_NOT_BINARY_OPERATOR_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #lhs, #op, #rhs);\
        embed::TEST_NOT_BINARY_OPERATOR_print_2((lhs), #op, (rhs), ((lhs) op (rhs)));\
    }\
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

#define TEST_TRUE(val) {\
    if(!(val)){\
        embed::TEST_TRUE_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val);\
        embed::TEST_TRUE_print_2(val);\
    }\
}

#define TEST_FALSE(val) {\
    if(val){\
        embed::TEST_FALSE_print_1(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__, #val);\
        embed::TEST_FALSE_print_2(val);\
    }\
}

#define TEST_ERROR embed::TEST_ERROR_print(EMBED_FUNCTION_SIGNATURE, __FILE__, __LINE__)

#define TEST_THROW(callable) try{callable; TEST_ERROR;}catch(...){}


