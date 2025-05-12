#pragma once

#include <coroutine>

namespace embed
{
    
    /**
     * \brief Awaitable that waits for a function to return true
     * 
     * Useful if you need to wait for an condition or register value
     * 
     * Example:
     * ```cpp
     * co_await embed::TrueAwait([]{return ptr->to->register->bit;})
     * ```
     * 
     * \tparam Callable a callable that returns true
     */
    template<class Callable>
    class TrueAwait{
        Callable _callable;
        public:

        TrueAwait(Callable&& callable) : _callable(std::forward<Callable>(callable)){}

        /**
         * \brief Returns the value of the callable
         */
        constexpr bool await_ready() const noexcept {return this->_callable();}

        /**
         * \brief no operation. For coroutine machinery.
         */
        constexpr void await_resume() {}

        /**
         * @brief no operation. suspends always. For coroutine machinery.
         */
        constexpr void await_suspend(std::coroutine_handle<> handle) noexcept{}
    }

} // namespace embed
