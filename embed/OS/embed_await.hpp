#pragma once

// std
#include <concepts>
#include <type_traits>

// embed
#include <embed/Core/concepts.hpp>
#include <embed/OS/Coroutine.hpp>

namespace embed {

    /**
     * @brief A warapper class used to integrate awaitables into the `embed` eco-system
     * 
     * `embed` demands that objects that are `co_await`ed derive from `embed::AwaitableNode`, which will register itself by the root task and the
     * scheduler. This allows the scheduler to know not only 'what' is awaited, but also 'why' or 'how' it was awaited, which is used to calculate 
     * the schedule.
     * 
     * If an Awaitable does not register itself, the scheduler either does not know if the awaitable is ready and the task can be safely resumed
     * 
     * Thus types that do not derive from `embed::AwaitableNode` can be wrapped into `embed::AwaitableWrapper` by using the helper functions: `wrap_awaitable()`;
     *
     * Example:
     * ```
     * co_await wrap_awaitale(boost::Awaitable);
     * ```
     *  
     * @tparam T an awaitable like object that has to define the methods `.await_ready()` and `.await_resume()`
     * @tparam is_lvalue a boolean that tells wether or not the input is an rvalue. If it is, then the other awaitable will be moved and stored, if it is an lvalue, only a reference will be stored. 
     */
    template<embed::AwaitableLike T, bool is_rvalue = true>
    class AwaitableWrapper : embed::AwaitableNode{
    private:
        std::remove_cvref_t<T> _awaitable;
    public:
        constexpr AwaitableWrapper(T&& awaitable) : _awaitable(std::move(awaitable)){}
        constexpr bool await_ready() const noexcept override {return this->_awaitable.await_ready();}
        inline auto await_resume() noexcept {return this->_awaitable.await_resume();}
    };

    /**
     * @brief Specialisation of `AwaitableWrapper` for lvalue references
     */
    template<embed::AwaitableLike T>
    class AwaitableWrapper<T, false> : embed::AwaitableNode{
    private:
        std::remove_cvref_t<T>& _awaitable;
    public:
        constexpr AwaitableWrapper(T& awaitable) : _awaitable(awaitable){}
        constexpr bool await_ready() const noexcept override {return this->_awaitable.await_ready();}
        inline auto await_resume() noexcept {return this->_awaitable.await_resume();}
    };

    /**
     * @brief Wraps an awaitable into an embed comform awaitable by lvalue reference
     */
    template<embed::AwaitableLike T>
    AwaitableWrapper<T, false> wrap_awaitable(T& lvalue){
        return AwaitableWrapper<T, false>(lvalue);
    }

    /**
     * @brief Wraps an awaitable into an embed comform awaitable by rvalue reference
     */
    template<embed::AwaitableLike T>
    AwaitableWrapper<T, true> wrap_awaitable(T&& rvalue){
        return AwaitableWrapper<T, true>(std::move(rvalue));
    }
}
