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
        std::chrono::nanoseconds _delay_ready;
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
         * @see CoTaskSignal::ImplicitDelay
         * 
         * @param delay The delay by which the task will be re-scheduled
         */
        template<class Rep, CStdRatio Period>
        explicit constexpr Delay(std::chrono::duration<Rep, Period> delay)
            : _delay_ready(std::chrono::duration_cast<std::chrono::nanoseconds>(delay)){}

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
         * @see CoTaskSignal::ExplicitDelay
         * 
         * @param delay
         */
        template<class Rep1, CStdRatio Period1, class Rep2, CStdRatio Period2>
        explicit constexpr Delay(std::chrono::duration<Rep1, Period1> delay, std::chrono::duration<Rep2, Period2> relative_deadline)
            : _delay_ready(std::chrono::duration_cast<std::chrono::nanoseconds>(delay))
            , _delay_deadline(std::chrono::duration_cast<std::chrono::nanoseconds>(relative_deadline)){}
            
        inline bool await_ready() const noexcept override {return this->_ready;;}

        inline void await_resume() noexcept {}

    private:
        inline CoTaskSignal await_suspend_signal() noexcept override {
            this->_ready = true;
            if(this->_delay_deadline.has_value()){
                return CoTaskSignal().explicit_delay(this->_delay_ready, this->_delay_deadline.value());
            }else{
                return CoTaskSignal().implicit_delay(this->_delay_ready);
            }
        }
    };

} // namespace embed
