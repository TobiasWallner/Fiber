#pragma once

#include <chrono>

#include "embed/Exceptions.hpp"

using namespace std::chrono_literals;

namespace embed
{

    /** 
     * @brief Yields/Suspends execution and passes CPU to another task.
     * @note if the other task takes too long and is not cooperative enough to yield itself, this task may miss its deadline
     * */
    class Yield{};

    /**
     * @brief Suspends execution, for the passed time, without ending the cycle
     * @note Since the cycle is not ended but continues, if you pass too long of a delay is passed, the task may miss its deadline.
     * To prevent this consider `YieldRemaining` instead which uses a delay in permil relative to the remaining deadline time.
     */
    class YieldDelay{
    private:        
        std::chrono::nanoseconds _intra_cycle_delay;
    public:
        template<class Rep, class Period = std::ratio<1>>
        constexpr explicit YieldDelay(std::chrono::duration<Rep, Period> delay) : _intra_cycle_delay(std::chrono::duration_cast<std::chrono::nanoseconds>(delay)){}
        constexpr std::chrono::nanoseconds intra_cycle_delay() const {return this->_intra_cycle_delay;}
    };

    /**
     * @brief Suspends execution, for the passed time represented in permil from now to the deadline, without ending the cycle.
     * @note if the other task takes too long and is not cooperative enough to yield itself, this task may miss its deadline
     */
    class YieldRemaining{
        private:
            uint32_t _permil_delay;
        public:
            constexpr explicit YieldRemaining(uint32_t permil_delay) : _permil_delay(permil_delay){}
            constexpr uint32_t permil_delay() const {return this->_permil_delay;}
        };
} // namespace embed
