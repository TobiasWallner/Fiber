#pragma once

#include <embed/Core/concepts.hpp>
#include <embed/OS/Coroutine.hpp>

/**
 * @brief Replaces `co_await` suspension points with a safe alternative.
 * 
 * `co_await` can sadly not be overloaded to check if an awaited object coforms to certain rules.
 * In order for an awaitable ot work together with embedOS's schedulers they have to derive from `embed::AwaitableNode`,
 * which performs all the necessary registrations and signaling from the coroutines leaf execution to the scheduler.
 * 
 * embed_await() will check, if an awaitable conforms to the `embedOS` eco-system. 
 *  - If yes, then nothing happens and the `embed::` awaitable is forwarded to `co_await`.
 *  - If not, then the awaitable will be wrapped to add the `embed::AwaitableNode`
 * 
 * This allows the integration and interoperability of awaitables from external libraries like: 
 * 
 * 
 * If you strictly and disciplinly only use `embedOS` and its awaitable types, then you can also just use 
 * the raw `co_await`. However, then you do not have the guard rails that `embed_await()` adds
 * 
 * @param awaitable Any awaitable object that defines the methods: `.await_ready()` and `.await_resume()`
 */
#define embed_await(awaitable) co_await embed::embed_awaitable_wrapper(awaitable)

namespace embed {

    /**
     * @brief A warapper class used with the macro `embed_await(awaitable)` to integrate non-conforming awaitables into the `embed` eco-system
     * 
     * This class is part of the 3 stages: `embed_await(awaitable)` --> `embed::embed_awaitable_wrapper()` --> AwaitableShim.
     * 
     * `embed` demands that objects that are `co_await`ed derive from `embed::AwaitableNode`, which will register itself by the root task and the
     * scheduler. This allows the scheduler to know not only 'what' is awaited, but also 'why' or 'how' it was awaited, which is used to calculate 
     * the schedule.
     * 
     * If an Awaitable does not register itself, the scheduler either does not know if the awaitable is ready and the task can be safely resumed
     * 
     * Thus types that do not derive from `embed::AwaitableNode` will be wrapped into `embed::AwaitableShim` which adds it.
     * 
     * @tparam T an awaitable like object that has to define the methods `.await_ready()` and `.await_resume()`
     */
    template<embed::AwaitableLike T>
    class AwaitableShim : embed::AwaitableNode{
    public:
        using value_type = std::remove_cvref_t<T>;
    
    private:
        value_type _awaitable;

    public:

        constexpr AwaitableShim(T&& awaitable) : _awaitable(std::move(awaitable)){}

        constexpr bool await_ready() const noexcept override {return this->_awaitable.await_ready();}
        inline auto await_resume() noexcept {return this->_awaitable.await_resume();}

    };

    /**
     * @brief used by the macro `embed_await()` to wrapp awaitables and add `embed::AwaitableNode` if it does not already for seemless integration and interoperatbility into the embed async eco-system.
     */
    template<embed::AwaitableLike T>
    requires (std::derived_from<std::remove_cvref_t<T>, embed::AwaitableNode> || std::derived_from<std::remove_cvref_t<T>, embed::CoroutineNode>)
    constexpr T&& embed_awaitable_wrapper(T&& awaitable) noexcept {
        return std::forward<T>(awaitable);
    }

    /**
     * @brief used by the macro `embed_await()` to wrapp awaitables and add `embed::AwaitableNode` if it does not already for seemless integration and interoperatbility into the embed async eco-system.
     */
    template<embed::AwaitableLike T>
    requires (!(std::derived_from<std::remove_cvref_t<T>, embed::AwaitableNode> || std::derived_from<std::remove_cvref_t<T>, embed::CoroutineNode>))
    auto embed_awaitable_wrapper(T&& awaitable) {
        return AwaitableShim<std::remove_cvref_t<T>>(std::forward<T>(awaitable));
    }

}