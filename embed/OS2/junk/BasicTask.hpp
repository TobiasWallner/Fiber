#pragma once

#include "Awaitable.hpp"

namespace embed{
    
    /**
     * @brief Container for coroutines
     * 
     * A `CoTask` is a container for coroutines. 
     * Coroutines are handled like nodes in a backwards linked list where every node knows its parent.
     * Additionally every new `CoroutineNode` will register itself in the `CoTask` as the new leaf on construction, so the container will always resume the tail/leaf of the linked list.
     * Further, every `CoroutineNode` will also un-register itself on destruction and re-register its master which is the coroutine that started the nested one.
     * 
     * Every awaited `CoroutineNode` will suspend to the `CoTask` and the `CoTask` is also the onely point that will `.resume()` the leaf/tail coroutine
     */
    class CoTask : public AwaitableNode{
    private:
        Coroutine<embed::Exit> _main_coroutine;
        std::coroutine_handle<> _leaf_coroutine;
        AwaitableNode* _leaf_awaitable;
        bool _instant_resume = false;
    public:

        /// @brief Construct from coroutine
        /// @param main the root coroutine of the coroutine linked list that this task will contain
        CoTask(Coroutine<embed::Exit>&& main) noexcept;
        CoTask(const CoTask&)=delete;
        CoTask(CoTask&& other) noexcept; 
        CoTask& operator=(const CoTask&)=delete;
        CoTask& operator=(CoTask&& other) noexcept;

        inline CoTask::~CoTask(){this->_main_coroutine.destroy();}

        /// @brief Registers the leaf nested coroutine that serves as the resume point after suspensions
        /// @details Meant to be called by the Coroutine
        /// @param leaf_handle a coroutine handle to the leaf tasks
        inline void register_leaf(std::coroutine_handle<void> leaf_handle) noexcept;

        /// @brief Registers a leaf awaitable that will be waited on before resuming its parents coroutine
        /// @details Meant to be called by the Awaitable
        /// @param awaitable The awaitable that will be waited on
        inline void register_leaf(AwaitableNode* awaitable) noexcept;

        
        /// @brief returns `true` if the awaitable that this task is waiting on is ready and `.resume()` can be called again
        /// @details also returns `true` if there is currently no awaitable that is being waited on.
        inline bool await_ready() const;

        /// @brief Safely resumes the task/coroutine
        /// @details This the only resume point for the whole coroutine chain. Every coroutine and nested coroutine suspends to here and will be executed/resumed from here by resuming the leaf/tail of the coroutine linked list.
        /// @return `true` if the coroutine was resumed, `fals` if it is still suspended and waiting on `await_ready()` 
        inline bool resume();

        /// @brief Returns `true` if the Task and all its coroutines are done 
        inline bool is_done() const {return this->_main_coroutine.is_done();}

        /// @brief Returns `true` if the Task and all its coroutines are done
        inline bool await_ready() const noexcept override {return this->_main_coroutine.is_done();}
    };

}