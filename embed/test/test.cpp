#include "test.hpp"

// embed-tests
#include <embed/Containers/tests/ArrayList_test.hpp>
#include <embed/Containers/tests/DualArrayList_test.hpp>
#include <embed/OS/tests/Clock_test.hpp>
#include <embed/OS/tests/Future_test.hpp>
#include <embed/OS/tests/Coroutine_test.hpp>
#include <embed/OS/tests/RealTimeScheduler_test.hpp>
#include <embed/OStream/tests/OStream_test.hpp>

#include <iostream>

int test_all(){

    #ifndef EMBED_DISABLE_EXCEPTIONS
    try{
    #endif
        return embed::TestResult()
            | embed::ArrayList_test
            | embed::DualArrayList_test
            | embed::ClockTick_test
            | embed::Future_test
            | embed::Coroutine_test
            | embed::RealTimeScheduler_test
            | embed::evaluate
            ;
    #ifndef EMBED_DISABLE_EXCEPTIONS
    }catch(embed::Exception& e){
        embed::cout << e << embed::endl;
    }catch(std::exception& e){
        embed::cout << e.what() << embed::endl;
    }
    return EXIT_FAILURE;
    #endif
}