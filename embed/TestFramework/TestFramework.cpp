#include "TestFramework.hpp"

namespace embed{
    

    int evaluate(TestResult result){
        // Test Summary: [PASS/FAIL]
        embed::cout << embed::newl << "Test Summary: " << embed::ansi::bold;
        if(result.failed == 0){
            embed::cout << embed::ansi::bright_green << "PASS";
        }else{
            embed::cout << embed::ansi::bright_red << "FAIL";
        }
        embed::cout << embed::ansi::reset << embed::newl;

        // Statistics: 
        embed::cout << "-----------------" << embed::newl;
        embed::cout << "passed: " << result.passed << embed::newl;
        embed::cout << "failed: " << result.failed << embed::newl;
        embed::cout << "-----------------" << embed::newl;
        embed::cout << embed::endl;
        return (result.failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    static constexpr const char* expected_true_but  = "    expected `true`  but: `";
    static constexpr const char* expected_false_but = "    expected `false` but: `";
                                                      

    void print_failed_function_file_line(const char* signature, const char* file, std::size_t line){
        embed::cerr << '[' << embed::ansi::bright_red << embed::ansi::bold << "Failed" << embed::ansi::reset << "]: test function" << embed::newl;
        embed::cerr << "    in: " << embed::ansi::magenta << signature << embed::ansi::reset << embed::newl;
        embed::cerr << "    at: " << embed::ansi::grey << file << ':' << line << embed::ansi::reset << embed::newl;
    }

    void TEST_FUNC_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* val_str){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_true_but << test_func << '(' << val_str << ")`" << embed::newl;
    }
    
    void TEST_FUNC2_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* lhs_str, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_true_but << test_func << '(' << lhs_str << ", " << rhs_str << ")`" << embed::newl;
    }
    
    void TEST_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_true_but << lhs_str << ' ' << op << ' ' << rhs_str << '`' << embed::newl;
    }

    void TEST_NOT_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_false_but << lhs_str << ' ' << op << ' ' << rhs_str << '`' << embed::newl;
    }

    void TEST_TRUE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_true_but << val << '`' << embed::newl;
    }

    void TEST_FALSE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << expected_false_but << val << '`' << embed::newl;
    }

    void TEST_ERROR_print(const char* signature, const char* file, std::size_t line){
        print_failed_function_file_line(signature, file, line);
        embed::cerr << embed::ansi::yellow << "    This should not have been reached" << embed::ansi::reset << embed::newl << embed::endl;
    }
    
}