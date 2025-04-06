#pragma once

#include <chrono>
#include <optional>

#include "embed/Exceptions.hpp"

using namespace std::chrono_literals;

namespace embed
{

    /// @brief Ends the cycle and causes `update_schedule()` to be called
    class Cycle{};

    /// @brief Ends the cycle and schedules it to `start = cycle_start_delay + prev_start`, `deadline = cycle_end_delay + prev_deadline`.
    class CycleAbsoluteDelay{
    private:
        std::chrono::nanoseconds _cycle_start_delay = 0ns;
        std::chrono::nanoseconds _cycle_deadline_delay = 0ns;

    public:
        constexpr CycleAbsoluteDelay(std::chrono::nanoseconds cycle_start_delay) 
            : _cycle_start_delay(cycle_start_delay)
            , _cycle_deadline_delay(cycle_start_delay){}

        constexpr CycleAbsoluteDelay(std::chrono::nanoseconds cycle_start_delay, std::chrono::nanoseconds cycle_deadline) 
            : _cycle_start_delay(cycle_start_delay)
            , _cycle_deadline_delay(cycle_deadline){}


        constexpr std::chrono::nanoseconds cycle_start_delay() const {return this->_cycle_start_delay;}
        constexpr std::chrono::nanoseconds cycle_deadline_delay() const {return this->_cycle_deadline_delay;}
    };

    /// @brief Ends the cycle and schedules it to `start = cycle_start_delay + now`, `deadline = cycle_end_delay + now`.
    class CycleRelativeDelay{
    private:
        std::chrono::nanoseconds _cycle_start_delay = 0ns;
        std::chrono::nanoseconds _cycle_deadline_delay = 0ns;

    public:
        
        constexpr CycleRelativeDelay(std::chrono::nanoseconds cycle_start_delay) 
            : _cycle_start_delay(cycle_start_delay)
            , _cycle_deadline_delay(cycle_start_delay){}

        constexpr CycleRelativeDelay(std::chrono::nanoseconds cycle_start_delay, std::chrono::nanoseconds cycle_deadline) 
            : _cycle_start_delay(cycle_start_delay)
            , _cycle_deadline_delay(cycle_deadline){}


        constexpr std::chrono::nanoseconds cycle_start_delay() const {return this->_cycle_start_delay;}
        constexpr std::chrono::nanoseconds cycle_deadline_delay() const {return this->_cycle_deadline_delay;}
    };


} // namespace embed

