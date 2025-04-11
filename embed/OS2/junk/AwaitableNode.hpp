#pragma once

namespace embed
{
        
    // forward declare
    class CoTask;

    class AwaitableNode{
    private:
        CoTask* master = nullptr;

    public:
        virtual ~AwaitableNode() noexcept = default;
        /*
            Coroutine expansion of `co_await`:
            ```
            auto&& awaitable = expr;
            if (!awaitable.await_ready()) {
                awaitable.await_suspend(current_coroutine_handle);
                co_return;
            }
            auto result = awaitable.await_resume();
            ```
        */

        virtual bool await_ready() const noexcept = 0;

        /// @brief Appends itself to the existing linked list of coroutines and registers itself (node) as the new leaf/tail of the list by the master (container)
        /// @tparam ReturnType Generic return type for custom coroutines with custom returns
        /// @param handle A handle that has to be an `embed::CoroutinePromise` but can have any return type. 
        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept; // defined after basic task
    };

} // namespace embed

#include "CoTask.hpp" // include here to satisfy class completeness for recursive class dependencies.

namespace embed
{
    /// @brief Appends itself to the existing linked list of coroutines and registers itself (node) as the new leaf/tail of the list by the master (container)
    /// @tparam ReturnType Generic return type for custom coroutines with custom returns
    /// @param handle A handle that has to be an `embed::CoroutinePromise` but can have any return type. 
    template<class ReturnType>
    inline void AwaiableNode::await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept {
        // store the master
        this->master = handle.promise().future().master;

        // register leaf in master to tell it what awaitable to wait for
        this->master->register_leaf(this);
    }
} // namespace embed

    
