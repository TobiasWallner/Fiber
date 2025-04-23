#pragma once

#include <embed/TestFramework/TestFramework.hpp>

namespace embed{

    /**
     * @brief Tests ClockTick and instantiations of derived Duration, TimePoint, CClock and Clock
     * 
     * Depends on `OStream`, `embed::cerr` and `embed::cout`.
     */
    embed::TestResult ClockTick_test();
    
} // namespace embed
