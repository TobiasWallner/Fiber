#pragma once

// std
#include <concepts>
#include <coroutine>

namespace fiber
{

    /**
     * \brief Concept describeing the interface of an object that can be `co_awaited`.
     */
    template<class Awaitable, class ResultType = void>
    class cAwaitable = requires(Awaitable awaitable){

        { awaitable.await_ready() } -> std::convertible_to<bool>;

        template<class T>
        { awaitable.await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) } -> std::same_as<bool> || std::same_as<void>;
        
        { awaitable.await_resume() } -> std::same_as<ResultType>;
    };

} // namespace fiber
