#pragma once

// std
#include <chrono>
#include <optional>

// embed
#include <embed/OS/Coroutine.hpp>
#include <embed/Core/concepts.hpp>

namespace embed
{
    
    class Delay : public AwaitableNode{
        std::chrono::nanoseconds _delay_ready = std::chrono::nanoseconds(0);
        std::optional<std::chrono::nanoseconds> _delay_deadline = std::nullopt;
        bool _ready = false;
    public:

        /**
         * @brief Constructs a delay that can be `co_await`ed and sends a signal to the scheduler for re-scheduling.
         * 
         * Re-schedules the task with the delay relative to now and infers the deadline from the previous execution.
         * 
         * The next ready time:
         * ```
         * next_ready = Clock::now() + delay;
         * ```
         * 
         * The next deadline: 
         * ```
         * next_deadline = Clock::now() + delay + (previous_deadline - previous_ready)
         * ```
         * 
         * @see CoSignal::ImplicitDelay
         * 
         * @param delay The delay by which the task will be re-scheduled
         */
        template<class Rep, CStdRatio Period>
        explicit constexpr Delay(std::chrono::duration<Rep, Period> delay)
            : _delay_ready(embed::rounding_duration_cast<std::chrono::nanoseconds>(delay)){}

        /**
         * @brief Constructs a delay that can be `co_await`ed and sends a signal to the scheduler for re-scheduleing.
         * 
         * Re-schedules the task with the delay relative to now and the relative_deadline relative to the new ready time.
         * 
         * The next ready time:
         * ```
         * next_ready = Clock::now() + delay;
         * ```
         * 
         * The next deadline: 
         * ```
         * next_deadline = Clock::now() + delay + relative_deadline
         * ```
         * 
         * @tparam Rep1 The representation of the std::chrono::duration, usually an integer type
         * @tparam Period1 The period of the std::chrono::duration. Has to be a std::ratio
         * @tparam Rep2 The representation of the std::chrono::duration, usually an integer type
         * @tparam Period2 The period of the std::chrono::duration. Has to be a std::ratio
         * @param delay The delay by which this task should be delayed
         * @param relative_deadline The added deadline after the delay
         * @see CoSignal::ExplicitDelay
         */
        template<class Rep1, CStdRatio Period1, class Rep2, CStdRatio Period2>
        explicit constexpr Delay(std::chrono::duration<Rep1, Period1> delay, std::chrono::duration<Rep2, Period2> relative_deadline)
            : _delay_ready(std::chrono::duration_cast<std::chrono::nanoseconds>(delay))
            , _delay_deadline(std::chrono::duration_cast<std::chrono::nanoseconds>(relative_deadline)){}
        
        /// @brief return `true` if the awatiable is ready. 
        /// @details initially returns `false` on the first read, but `true` on the second. `_ready` is set `true` in `await_suspend_signal()`
        inline bool await_ready() const noexcept override {return this->_ready;}

        /// @brief A delay does not return a value to be read -> `void`
        inline void await_resume() noexcept {}

    private:

        /// @brief Signals a delay to the root task or scheduler and sets `_ready` to `true`.
        inline CoSignal await_suspend_signal() noexcept override {
            this->_ready = true;
            CoSignal signal;
            if(this->_delay_deadline.has_value()){
                signal.explicit_delay(this->_delay_ready, this->_delay_deadline.value());
            }else{
                signal.implicit_delay(this->_delay_ready);
            }
            return signal;
        }
    };

} // namespace embed
