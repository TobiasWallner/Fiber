#include "test.hpp"

namespace embed{
    
    static constexpr const char* failed_function    = "[ Failed ]: Function: ";
    static constexpr const char* in_file            = "            in File : '";
    static constexpr const char* at_line            = "' at line: ";
    static constexpr const char* expected_true_but  = "            expected `true`  but: `";
    static constexpr const char* expected_false_but = "            expected `false` but: `";
    


    void print_failed_function_file_line(const char* signature, const char* file, std::size_t line){
        embed::cout << failed_function << signature << embed::newl;
        embed::cout << in_file << file << at_line << line << embed::newl;
    }

    void TEST_FUNC_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* val_str){
        print_failed_function_file_line(signature, file, line);
        embed::cout << expected_true_but << test_func << '(' << val_str << ")`" << embed::newl;
    }
    
    void TEST_FUNC2_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* lhs_str, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        embed::cout << expected_true_but << test_func << '(' << lhs_str << ", " << rhs_str << ")`" << embed::newl;
    }
    
    void TEST_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        embed::cout << expected_true_but << lhs_str << ' ' << op << ' ' << rhs_str << '`' << embed::newl;
    }

    void TEST_TRUE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        embed::cout << expected_true_but << val << '`' << embed::newl;
    }

    void TEST_FALSE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        embed::cout << expected_false_but << val << '`' << embed::newl;
    }

    void TEST_ERROR_print(const char* signature, const char* file, std::size_t line){
        print_failed_function_file_line(signature, file, line);
        embed::cout << "            This should not have been reached" << embed::newl << embed::endl;
    }
    
}