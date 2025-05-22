#include "test.hpp"

// fiber-tests
#include <fiber/Containers/tests/ArrayList_test.hpp>
#include <fiber/Containers/tests/DualArrayList_test.hpp>
#include <fiber/Chrono/tests/Clock_test.hpp>
#include <fiber/Future/tests/Future_test.hpp>
#include <fiber/OS/tests/Coroutine_test.hpp>
#include <fiber/OS/tests/Scheduler_test.hpp>
#include <fiber/OStream/tests/OStream_test.hpp>

#include <iostream>

#include <fiber/Future/Future.hpp>
#include <fiber/OS/Coroutine.hpp>

int test_all(){

    #ifndef FIBER_DISABLE_EXCEPTIONS
    try{
    #endif
        return fiber::TestResult()
            | fiber::ArrayList_test
            | fiber::DualArrayList_test
            | fiber::ClockTick_test
            | fiber::Future_test
            | fiber::Coroutine_test
            | fiber::Scheduler_test
            | fiber::evaluate
            ;
    #ifndef FIBER_DISABLE_EXCEPTIONS
    }catch(fiber::Exception& e){
        fiber::cout << e << fiber::endl;
    }catch(std::exception& e){
        fiber::cout << e.what() << fiber::endl;
    }
    return EXIT_FAILURE;
    #endif
}