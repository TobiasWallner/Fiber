#pragma once

#include <fiber/TestFramework/TestFramework.hpp>

namespace fiber{

    /**
     * @brief Tests Tick and instantiations of derived Duration, TimePoint, CClock and Clock
     * 
     * Depends on `OStream`, `fiber::cerr` and `fiber::cout`.
     */
    fiber::TestResult ClockTick_test();
    
} // namespace fiber
