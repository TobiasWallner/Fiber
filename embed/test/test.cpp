#include "test.hpp"

// embed-tests
#include <embed/Containers/tests/ArrayList_test.hpp>
#include <embed/Containers/tests/DualArrayList_test.hpp>
#include <embed/OS/tests/Clock_test.hpp>
#include <embed/OS/tests/Future_test.hpp>
#include <embed/OS/tests/Coroutine_test.hpp>

int test_all(){
    try{
        return embed::TestResult()
            | embed::ArrayList_test
            | embed::DualArrayList_test
            | embed::ClockTick_test
            | embed::Future_test
            | embed::Coroutine_test
            | embed::evaluate
            ;

    }catch(embed::Exception& e){
        embed::cout << e << embed::endl;
    }catch(std::exception& e){
        embed::cout << e.what() << embed::endl;
    }
    return EXIT_FAILURE;
}