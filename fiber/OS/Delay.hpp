#pragma once

// std
#include <chrono>
#include <optional>

// fiber
#include <fiber/Chrono/Duration.hpp>
#include <fiber/OS/Coroutine.hpp>
#include <fiber/Core/concepts.hpp>

namespace fiber
{
    
    class Delay{
        const fiber::Duration _delay_ready;
        const fiber::Duration _delay_deadline;
        const bool _has_deadline = false;
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
        constexpr Delay(fiber::Duration delay)
            : _delay_ready(delay){}

        /**
         * \brief Constructs a delay that can be `co_await`ed and sends a signal to the scheduler for re-scheduling **with exposed rounding type**
         */
        template<RoundingMethod rounding_type = RoundingMethod::Nearest, class Rep = unsigned int, CRatio Period = std::ratio<1>>
        constexpr Delay(std::chrono::duration<Rep, Period> delay)
            : Delay(fiber::Duration<rounding_type>(delay)){}

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
         * @param delay The delay by which this task should be delayed
         * @param relative_deadline The added deadline after the delay
         * @see CoSignal::ExplicitDelay
         */
        constexpr Delay(fiber::Duration delay, fiber::Duration relative_deadline)
            : _delay_ready(delay)
            , _delay_deadline(relative_deadline)
            , _has_deadline(true){}
        
        /// @brief return `true` if the awatiable is ready. 
        /// @details initially returns `false` on the first read, but `true` on the second. `_ready` is set `true` in `await_suspend_signal()`
        constexpr bool await_ready() const noexcept {return false;}

        /// @brief A delay does not return a value to be read -> `void`
        constexpr void await_resume() const noexcept {}

        template<class ReturnType>
        constexpr void await_suspend(std::coroutine_handle<fiber::CoroutinePromise<ReturnType>> handle) const noexcept {
            // do not register --> task stays resumable
            // handle.promise().task()->register_leaf(this, Delay::s_await_ready);

            // optionally send a signal to the task
            handle.promise().task()->signal(await_suspend_signal());
        }
    private:
        /// @brief Signals a delay to the root task or scheduler and sets `_ready` to `true`.
        constexpr CoSignal await_suspend_signal() const noexcept {
            CoSignal signal;
            if(this->_has_deadline){
                signal.explicit_delay(this->_delay_ready, this->_delay_deadline);
            }else{
                signal.implicit_delay(this->_delay_ready);
            }
            return signal;
        }
    };

} // namespace fiber
