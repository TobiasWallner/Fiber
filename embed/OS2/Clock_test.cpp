//std
#include <cstdint>
#include <limits>
#include <ranges>

//embed
#include "embed/OS2/Clock.hpp"
#include "embed/OS2/Clock_test.hpp"
#include "embed/test.hpp"



namespace embed{

    static void ClockTick_test_construction(){
        {// natural construction
            ClockTick<uint16_t> tick(uint8_t(100));
            TEST_EQUAL(tick.value, 100);
        }
        {// construction with modulo that is a power of two
            ClockTick<uint16_t, (1<<10)-1> tick1(uint8_t(100));
            TEST_EQUAL(tick1.value, 100);

            ClockTick<uint16_t, (1<<10)-1> tick2(uint32_t((1<<11) | (1<<10) | (1<<9)));
            TEST_EQUAL(tick2.value, (1<<9));
        }
        {// construction where the half the input integer is smaller than the modulo
            ClockTick<uint8_t, 200-1> tick1(uint8_t(70));
            TEST_EQUAL(tick1.value, 70);

            ClockTick<uint8_t, 200-1> tick2(uint8_t(210));
            TEST_EQUAL(tick2.value, 10);
        }
        {// construction where an actual '%' operation has to be used
            ClockTick<uint32_t, 55-1> tick1(uint32_t(42));
            TEST_EQUAL(tick1.value, 42);
            
            ClockTick<uint32_t, 55-1> tick2(uint32_t(60));
            TEST_EQUAL(tick2.value, 5);
        }
        {// construct from negative integer
            ClockTick<uint32_t, 1000-1> tick(-1);
            TEST_EQUAL(tick.value, 999);
        }
    }

    static void ClockTick_test_addition(){
        {// natural overflow
            ClockTick<uint32_t> a(455UL);
            ClockTick<uint32_t> b(32165UL);
            TEST_EQUAL(a + b, 455UL + 32165UL);
        }
        {
            ClockTick<uint32_t> a(455UL);
            ClockTick<uint32_t> b(std::numeric_limits<uint32_t>::max());
            TEST_EQUAL(a + b, 454UL);
        }

        {// overflow with power of two
            ClockTick<uint32_t, 1024-1> a(455UL);
            ClockTick<uint32_t, 1024-1> b(200UL);
            TEST_EQUAL(a + b, 455UL + 200UL)
        }
        {
            ClockTick<uint32_t, 1024-1> a(455UL);
            ClockTick<uint32_t, 1024-1> b(800UL);
            TEST_EQUAL(a + b, (455UL + 800UL) % 1024);
        }

        {// fallback overflow with conditional subtraction
            ClockTick<uint32_t, 987-1> a(455UL);
            ClockTick<uint32_t, 987-1> b(200UL);
            TEST_EQUAL(a + b, 455UL + 200UL)
        }
        {
            ClockTick<uint32_t, 987-1> a(455UL);
            ClockTick<uint32_t, 987-1> b(800UL);
            TEST_EQUAL(a + b, (455UL + 800UL) % 987);
        }
    }

    static void ClockTick_test_subtraction(){
        {// natural overflow subtraction
            ClockTick<uint8_t> a(50UL);
            ClockTick<uint8_t> b(18UL);
            TEST_EQUAL(a - b, 50UL - 18UL);
        }
        {
            ClockTick<uint8_t> a(uint8_t(50));
            ClockTick<uint8_t> b(uint8_t(18));
            uint8_t expected(18);
            expected -= uint8_t(50);
            TEST_EQUAL(b - a, expected);
        }
        
        {// un-natural overflow with conditional subtraction
            ClockTick<uint8_t, 100-1> a(50UL);
            ClockTick<uint8_t, 100-1> b(18UL);
            TEST_EQUAL(a - b, 50UL - 18UL);
        }
        {
            ClockTick<uint8_t, 100-1> a(uint8_t(50));
            ClockTick<uint8_t, 100-1> b(uint8_t(18));
            uint8_t expected = 18;
            expected -= 50;
            expected += 100;
            TEST_EQUAL(b - a, expected);
        }
    }

    static void ClockTest_negation(){
        {
            ClockTick<uint16_t, 1024-1> v(577UL);
            TEST_EQUAL(-v, 1024UL - 577UL);
        }
        {
            ClockTick<uint32_t, 524-1> v(42UL);
            TEST_EQUAL(-v, 524UL - 42UL);
        }
        {
            ClockTick<uint8_t, 200-1> v(56UL);
            TEST_EQUAL(-v, 200UL - 56UL);
        }
        {
            ClockTick<uint8_t> v(56UL);
            TEST_EQUAL(-v, 256UL - 56UL);
        }
    }

    static void ClockTick_test_comparison(){
        // test <=
        for(unsigned int i : std::views::iota(0, 256)){
            for(unsigned int j : std::views::iota(i, i+127)){
                ClockTick<uint8_t> a(i);
                ClockTick<uint8_t> b(j);
                TEST_SMALLER_EQUAL(a, b);
            }
            for(unsigned int j : std::views::iota(i+127, i+256)){
                ClockTick<uint8_t> a(i);
                ClockTick<uint8_t> b(j);
                TEST_NOT_SMALLER_EQUAL(a, b);
            }
        }

        for(unsigned long long i = 0; i < (1ULL<<31ULL); i+=4'294'967ULL){
            for(unsigned long long j = i; j < (i+(1ULL<<31ULL)); j+=4'294'967ULL){
                ClockTick<uint32_t> a(i);
                ClockTick<uint32_t> b(j);
                TEST_SMALLER_EQUAL(a, b);
            }
            for(unsigned long long j = i + (1ULL<<31ULL); j < (i + (1ULL<<32ULL)); j+=4'294'967ULL){
                ClockTick<uint32_t> a(i);
                ClockTick<uint32_t> b(j);
                TEST_NOT_SMALLER_EQUAL(a, b); // !!! Why dafuq is there a minus and why dafuq does it fail !!!
            }
        }

        TEST_GREATER_EQUAL((ClockTick<uint32_t, 1024-1>(0UL)), (ClockTick<uint32_t, 1024-1>(0UL)));
        TEST_GREATER_EQUAL((ClockTick<uint32_t, 1024-1>(1UL)), (ClockTick<uint32_t, 1024-1>(0UL)));
        TEST_GREATER_EQUAL((ClockTick<uint32_t, 1024-1>(1UL)), (ClockTick<uint32_t, 1024-1>(1023UL)));

        TEST_NOT_GREATER_EQUAL((ClockTick<uint32_t, 1024-1>(600UL)), (ClockTick<uint32_t, 1024-1>(0UL)));
        TEST_NOT_GREATER_EQUAL((ClockTick<uint32_t, 1024-1>(1023UL)), (ClockTick<uint32_t, 1024-1>(1UL)));
    
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(0UL)), (ClockTick<uint32_t, 1000-1>(1UL)));
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(0UL)), (ClockTick<uint32_t, 1000-1>(300UL)));
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(0UL)), (ClockTick<uint32_t, 1000-1>(499UL)));
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(0UL)), (ClockTick<uint32_t, 1000-1>(499UL)));
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(900UL)), (ClockTick<uint32_t, 1000-1>(20UL)));
        TEST_SMALLER((ClockTick<uint32_t, 1000-1>(999UL)), (ClockTick<uint32_t, 1000-1>(400UL)));

        TEST_NOT_SMALLER((ClockTick<uint32_t, 1000-1>(0UL)), (ClockTick<uint32_t, 1000-1>(0UL)));
        TEST_NOT_SMALLER((ClockTick<uint32_t, 1000-1>(1UL)), (ClockTick<uint32_t, 1000-1>(0UL)));
        TEST_NOT_SMALLER((ClockTick<uint32_t, 1000-1>(999UL)), (ClockTick<uint32_t, 1000-1>(510UL)));
        
    }

    static void Duration_test_construction(){
        embed::Duration<uint32_t, std::nano, 1024> d;
    }

    static void Duration_test_std_integration(){
        embed::Duration<uint32_t, std::nano, 1024> d(5);
        const auto a = d + 1ns; // TODO: integrate std::numeric_literal into embed::Duration operators.
        TEST_EQUAL(a.count(), 6);
    }


    uint32_t get_timer_count(){return 1UL;}

    static void Clock_template_instantiation_and_time_point(){

        using MyClock = Clock<uint32_t, std::micro, get_timer_count>;

        using TimePoint = MyClock::time_point;

        TimePoint t1(1us);
        TimePoint t2(33us);

        TEST_SMALLER(t1, t2);
    }

    void ClockTick_test(){
        
        ClockTick_test_construction();
        ClockTick_test_addition();
        ClockTick_test_subtraction();
        ClockTest_negation();
        ClockTick_test_comparison();

        embed::cout << "============= finished ClockTicks test =============" << embed::endl;

        Duration_test_construction();
        Duration_test_std_integration();

        embed::cout << "============= finished Duration test =============" << embed::endl;

        Clock_template_instantiation_and_time_point();

        embed::cout << "============= finished Clock/Timepoint test =============" << embed::endl;

    }

}