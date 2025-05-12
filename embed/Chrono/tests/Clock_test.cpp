//std
#include <cstdint>
#include <limits>
#include <ranges>

//embed
#include "Clock_test.hpp"
#include "../Tick.hpp"
#include "../Duration.hpp"
#include "../TimePoint.hpp"
#include <embed/TestFramework/TestFramework.hpp>

namespace embed{

    static embed::TestResult ClockTick_test_construction(){
        TEST_START;

        {// natural construction
            Tick<uint16_t> tick(uint8_t(100));
            TEST_EQUAL(tick.value, 100);
        }
        {// construction with modulo that is a power of two
            Tick<uint16_t, (1<<10)-1> tick1(uint8_t(100));
            TEST_EQUAL(tick1.value, 100);

            Tick<uint16_t, (1<<10)-1> tick2(uint32_t((1<<11) | (1<<10) | (1<<9)));
            TEST_EQUAL(tick2.value, (1<<9));
        }
        {// construction where the half the input integer is smaller than the modulo
            Tick<uint8_t, 200-1> tick1(uint8_t(70));
            TEST_EQUAL(tick1.value, 70);

            Tick<uint8_t, 200-1> tick2(uint8_t(210));
            TEST_EQUAL(tick2.value, 10);
        }
        {// construction where an actual '%' operation has to be used
            Tick<uint32_t, 55-1> tick1(uint32_t(42));
            TEST_EQUAL(tick1.value, 42);
            
            Tick<uint32_t, 55-1> tick2(uint32_t(60));
            TEST_EQUAL(tick2.value, 5);
        }
        {// construct from negative integer
            Tick<uint32_t, 1000-1> tick(-1);
            TEST_EQUAL(tick.value, 999);
        }

        TEST_END;
    }

    static embed::TestResult ClockTick_test_addition(){
        TEST_START;

        {// natural overflow
            Tick<uint32_t> a(455UL);
            Tick<uint32_t> b(32165UL);
            TEST_EQUAL(a + b, 455UL + 32165UL);
        }
        {
            Tick<uint32_t> a(455UL);
            Tick<uint32_t> b(std::numeric_limits<uint32_t>::max());
            TEST_EQUAL(a + b, 454UL);
        }

        {// overflow with power of two
            Tick<uint32_t, 1024-1> a(455UL);
            Tick<uint32_t, 1024-1> b(200UL);
            TEST_EQUAL(a + b, 455UL + 200UL)
        }
        {
            Tick<uint32_t, 1024-1> a(455UL);
            Tick<uint32_t, 1024-1> b(800UL);
            TEST_EQUAL(a + b, (455UL + 800UL) % 1024);
        }

        {// fallback overflow with conditional subtraction
            Tick<uint32_t, 987-1> a(455UL);
            Tick<uint32_t, 987-1> b(200UL);
            TEST_EQUAL(a + b, 455UL + 200UL)
        }
        {
            Tick<uint32_t, 987-1> a(455UL);
            Tick<uint32_t, 987-1> b(800UL);
            TEST_EQUAL(a + b, (455UL + 800UL) % 987);
        }

        TEST_END;
    }

    static embed::TestResult ClockTick_test_subtraction(){
        TEST_START;

        {// natural overflow subtraction
            Tick<uint8_t> a(50UL);
            Tick<uint8_t> b(18UL);
            TEST_EQUAL(a - b, 50UL - 18UL);
        }
        {
            Tick<uint8_t> a(uint8_t(50));
            Tick<uint8_t> b(uint8_t(18));
            uint8_t expected(18);
            expected -= uint8_t(50);
            TEST_EQUAL(b - a, expected);
        }
        
        {// un-natural overflow with conditional subtraction
            Tick<uint8_t, 100-1> a(50UL);
            Tick<uint8_t, 100-1> b(18UL);
            TEST_EQUAL(a - b, 50UL - 18UL);
        }
        {
            Tick<uint8_t, 100-1> a(uint8_t(50));
            Tick<uint8_t, 100-1> b(uint8_t(18));
            uint8_t expected = 18;
            expected -= 50;
            expected += 100;
            TEST_EQUAL(b - a, expected);
        }

        TEST_END;
    }

    static embed::TestResult ClockTest_negation(){
        TEST_START;
        {
            Tick<uint16_t, 1024-1> v(577UL);
            TEST_EQUAL(-v, 1024UL - 577UL);
        }
        {
            Tick<uint32_t, 524-1> v(42UL);
            TEST_EQUAL(-v, 524UL - 42UL);
        }
        {
            Tick<uint8_t, 200-1> v(56UL);
            TEST_EQUAL(-v, 200UL - 56UL);
        }
        {
            Tick<uint8_t> v(56UL);
            TEST_EQUAL(-v, 256UL - 56UL);
        }
        TEST_END;
    }

    static embed::TestResult ClockTick_test_comparison(){
        TEST_START;
        // test <=
        for(unsigned int i : std::views::iota(0, 256)){
            for(unsigned int j : std::views::iota(i, i+127)){
                Tick<uint8_t> a(i);
                Tick<uint8_t> b(j);
                TEST_SMALLER_EQUAL(a, b);
            }
            for(unsigned int j : std::views::iota(i+127, i+256)){
                Tick<uint8_t> a(i);
                Tick<uint8_t> b(j);
                TEST_NOT_SMALLER_EQUAL(a, b);
            }
        }

        for(unsigned long long i = 0; i < (1ULL<<31ULL); i+=4'294'967ULL){
            for(unsigned long long j = i; j < (i+(1ULL<<31ULL)); j+=4'294'967ULL){
                Tick<uint32_t> a(i);
                Tick<uint32_t> b(j);
                TEST_SMALLER_EQUAL(a, b);
            }
            for(unsigned long long j = i + (1ULL<<31ULL); j < (i + (1ULL<<32ULL)); j+=4'294'967ULL){
                Tick<uint32_t> a(i);
                Tick<uint32_t> b(j);
                TEST_NOT_SMALLER_EQUAL(a, b); // !!! Why dafuq is there a minus and why dafuq does it fail !!!
            }
        }

        TEST_GREATER_EQUAL((Tick<uint32_t, 1024-1>(0UL)), (Tick<uint32_t, 1024-1>(0UL)));
        TEST_GREATER_EQUAL((Tick<uint32_t, 1024-1>(1UL)), (Tick<uint32_t, 1024-1>(0UL)));
        TEST_GREATER_EQUAL((Tick<uint32_t, 1024-1>(1UL)), (Tick<uint32_t, 1024-1>(1023UL)));

        TEST_NOT_GREATER_EQUAL((Tick<uint32_t, 1024-1>(600UL)), (Tick<uint32_t, 1024-1>(0UL)));
        TEST_NOT_GREATER_EQUAL((Tick<uint32_t, 1024-1>(1023UL)), (Tick<uint32_t, 1024-1>(1UL)));
    
        TEST_SMALLER((Tick<uint32_t, 1000-1>(0UL)), (Tick<uint32_t, 1000-1>(1UL)));
        TEST_SMALLER((Tick<uint32_t, 1000-1>(0UL)), (Tick<uint32_t, 1000-1>(300UL)));
        TEST_SMALLER((Tick<uint32_t, 1000-1>(0UL)), (Tick<uint32_t, 1000-1>(499UL)));
        TEST_SMALLER((Tick<uint32_t, 1000-1>(0UL)), (Tick<uint32_t, 1000-1>(499UL)));
        TEST_SMALLER((Tick<uint32_t, 1000-1>(900UL)), (Tick<uint32_t, 1000-1>(20UL)));
        TEST_SMALLER((Tick<uint32_t, 1000-1>(999UL)), (Tick<uint32_t, 1000-1>(400UL)));

        TEST_NOT_SMALLER((Tick<uint32_t, 1000-1>(0UL)), (Tick<uint32_t, 1000-1>(0UL)));
        TEST_NOT_SMALLER((Tick<uint32_t, 1000-1>(1UL)), (Tick<uint32_t, 1000-1>(0UL)));
        TEST_NOT_SMALLER((Tick<uint32_t, 1000-1>(999UL)), (Tick<uint32_t, 1000-1>(510UL)));
        TEST_END
    }

    static embed::TestResult Duration_test_construction(){
        TEST_START;
        [[maybe_unused]] embed::Duration d;
        TEST_END;
    }

    static embed::TestResult Duration_test_std_integration(){
        TEST_START;
        embed::Duration d(5ms);
        const auto a = d + 1ms;
        TEST_EQUAL(a, 6ms);
        TEST_END;
    }


    uint32_t get_timer_count(){return 1UL;}

    static embed::TestResult Clock_template_instantiation_and_time_point(){
        TEST_START;

        using TimePoint = embed::TimePoint;

        TimePoint t1(1ms);
        TimePoint t2(33ms);

        TEST_SMALLER(t1, t2);
        
        TEST_END;
    }

    embed::TestResult ClockTick_test(){
        TEST_GROUP;
        
        return embed::TestResult()
            | ClockTick_test_construction
            | ClockTick_test_addition
            | ClockTick_test_subtraction
            | ClockTest_negation
            | ClockTick_test_comparison
            | Duration_test_construction
            | Duration_test_std_integration
            | Clock_template_instantiation_and_time_point;

    }

}