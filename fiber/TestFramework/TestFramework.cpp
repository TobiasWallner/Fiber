#include "TestFramework.hpp"

namespace fiber{
    

    int evaluate(TestResult result){
        // Test Summary: [PASS/FAIL]
        fiber::cout << fiber::newl << "Test Summary: " << fiber::ansi::bold;
        if(result.failed == 0){
            fiber::cout << fiber::ansi::bright_green << "PASS";
        }else{
            fiber::cout << fiber::ansi::bright_red << "FAIL";
        }
        fiber::cout << fiber::ansi::reset << fiber::newl;

        // Statistics: 
        fiber::cout << "-----------------" << fiber::newl;
        fiber::cout << "passed: " << result.passed << fiber::newl;
        fiber::cout << "failed: " << result.failed << fiber::newl;
        fiber::cout << "-----------------" << fiber::newl;
        fiber::cout << fiber::endl;
        return (result.failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    static constexpr const char* expected_true_but  = "    expected `true`  but: `";
    static constexpr const char* expected_false_but = "    expected `false` but: `";
                                                      

    void print_failed_function_file_line(const char* signature, const char* file, std::size_t line){
        fiber::cerr << '[' << fiber::ansi::bright_red << fiber::ansi::bold << "Failed" << fiber::ansi::reset << "]: test function" << fiber::newl;
        fiber::cerr << "    in: " << fiber::ansi::magenta << signature << fiber::ansi::reset << fiber::newl;
        fiber::cerr << "    at: " << fiber::ansi::grey << file << ':' << line << fiber::ansi::reset << fiber::newl;
    }

    void TEST_FUNC_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* val_str){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_true_but << test_func << '(' << val_str << ")`" << fiber::newl;
    }
    
    void TEST_FUNC2_print_1(const char* signature, const char* file, std::size_t line, const char* test_func, const char* lhs_str, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_true_but << test_func << '(' << lhs_str << ", " << rhs_str << ")`" << fiber::newl;
    }
    
    void TEST_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_true_but << lhs_str << ' ' << op << ' ' << rhs_str << '`' << fiber::newl;
    }

    void TEST_NOT_BINARY_OPERATOR_print_1(const char* signature, const char* file, std::size_t line, const char* lhs_str, const char* op, const char* rhs_str){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_false_but << lhs_str << ' ' << op << ' ' << rhs_str << '`' << fiber::newl;
    }

    void TEST_TRUE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_true_but << val << '`' << fiber::newl;
    }

    void TEST_FALSE_print_1(const char* signature, const char* file, std::size_t line, const char* val){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << expected_false_but << val << '`' << fiber::newl;
    }

    void TEST_ERROR_print(const char* signature, const char* file, std::size_t line){
        print_failed_function_file_line(signature, file, line);
        fiber::cerr << fiber::ansi::yellow << "    This should not have been reached" << fiber::ansi::reset << fiber::newl << fiber::endl;
    }
    
}