#pragma once

#include <fiber/OS/Coroutine.hpp>

namespace fiber
{
    
    class NextCycle{
    public:

        constexpr bool await_ready() const noexcept {return false;}

        constexpr void await_resume() const noexcept {}

        template<class ReturnType>
        constexpr void await_suspend(std::coroutine_handle<fiber::CoroutinePromise<ReturnType>> handle) const noexcept {
            // do not register --> Task stays resumable

            // optionally send a signal to the task
            handle.promise().task()->signal(await_suspend_signal());
        }

    private:
        constexpr CoSignal await_suspend_signal() const noexcept {
            return CoSignal().next_cycle();
        }
    };

} // namespace fiber
