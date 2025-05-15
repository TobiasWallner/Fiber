#pragma once

// std
#include <concepts>
#include <coroutine>

namespace fiber
{
    
    template<class ResultType = void>
    class iAwaitable{
        public:
        virtual bool await_ready() const = 0;
        virtual bool await_suspend(std::coroutine_handle<> handle) = 0;
        virtual ResultType await_resume() = 0;
    };

    template<class Awaitable, class ResultType = void>
    class cAwaitable = requires(Awaitable awaitable){
        { awaitable.await_ready() } -> std::same_as<bool>;
        { awaitable.await_suspend(std::coroutine_handle<> handle) } -> std::same_as<bool>;
        { awaitable.await_resume() } -> std::same_as<ResultType>;
    };

} // namespace fiber
