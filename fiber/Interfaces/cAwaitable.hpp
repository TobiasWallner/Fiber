#pragma once

// std
#include <concepts>
#include <coroutine>

namespace fiber
{

    // forward declaration
    template<class T>
    class CoroutinePromise;

    /**
     * \brief Concept describeing the interface of an object that can be `co_awaited`.
     * 
     * Implements the following functions:
     * - `bool await_ready()`
     * - `ResultType await_resume()` 
     * - `void await_suspend(std::coroutine_handle<CoroutinePromise<T>>)` or `void await_suspend(std::coroutine_handle<CoroutinePromise<T>>)`
     */
    template<class Awaitable, class ResultType = void>
    concept cAwaitable = requires(Awaitable awaitable){
        { awaitable.await_ready() } -> std::convertible_to<bool>;
        { awaitable.await_resume() } -> std::same_as<ResultType>;
    } && (
        requires(Awaitable awaitable, std::coroutine_handle<CoroutinePromise<int/*any type*/>> handle){
            { awaitable.await_suspend(handle) } -> std::same_as<void>;
        }
        ||
        requires(Awaitable awaitable, std::coroutine_handle<CoroutinePromise<int/*any type*/>> handle){
            { awaitable.await_suspend(handle) } -> std::same_as<bool>;
        }
    );

} // namespace fiber
