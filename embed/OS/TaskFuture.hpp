#pragma once

//std
#include <coroutine>

//embed
#include "Exit.hpp"
#include "Suspend.hpp"

namespace embed{

    // foreward declaration
    template<class ReturnType>
    class TaskPromise;

    template<class ReturnType>
    struct TaskFuture{
        using promise_type = TaskPromise<ReturnType>;
        using handle_type = std::coroutine_handle<promise_type>;
    
    private:
        handle_type coro = nullptr;

    public:

        /// @brief default constructor 
        TaskFuture() = default;

        /// @brief construct forom coroutine handle
        /// @param h the coroutine handle
        explicit TaskFuture(handle_type h) : coro(h) {}

        /// @brief deleted copy constructor 
        TaskFuture(const TaskFuture&) = delete;

        /// @brief deleted copy assignment 
        TaskFuture& operator=(const TaskFuture&) = delete;

        /// @brief is move constructible
        TaskFuture(TaskFuture&& other) noexcept : coro(other.coro) {
            other.coro = nullptr;
        }

        /// @brief is move assignable
        TaskFuture& operator=(TaskFuture&& other) noexcept {
            if (this != &other) {
                coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }

        /// @brief destructor
        ~TaskFuture() {
            if (coro) coro.destroy();
        }

        /// @brief resumes the coroutine
        void resume() const {
            if (coro) coro.resume();
        }

        /// @brief checks if the coroutine is done
        /// @return returns `true` if the coroutine is done
        bool done() const {
            return coro.done();
        }

        /// @brief returns the promised value
        auto& promise() const {
            return coro.promise();
        }

        /// @brief checks if the coroutine exists
        /// @returns `true` if the coroutine exists and `falst` otherwise (you would have to create the coroutine first then)
        explicit operator bool() const noexcept {
            return coro != nullptr;
        }
    };

    /**
     * @brief promise for the result of a `co_await`, `co_yield` or `co_return` in the coroutine function (`main`) of a Task.
     * 
     * Usage in a task function:
     * ```
     * auto async_value = dma_copy(...);
     * calculate();
     * co_await async_value; // yields execution - equivalent to `co_yield ESuspend::Yield;`
     * while(...){
     *      calculate();
     *      co_yield Suspend::Yield; // manually yield execution to other tasks
     * }
     * co_yield Suspend::Cycle; // manually suspend until the next cycle
     * calculate();
     * co_return Exit::Success; // task finished and will be removed from the scheduling queues
     * ```
     */
    template<class ReturnType>
    struct TaskPromise{
        ReturnType _return_result;
        embed::Suspend _yield_result;
        bool _is_awaiting = false;
        bool _is_yielding = true;

        inline auto get_return_object(){
            using handle = std::coroutine_handle<TaskPromise<ReturnType>>;
            return TaskFuture<ReturnType>{handle::from_promise(*this)};
        }

        inline auto initial_suspend() noexcept {return std::suspend_always{};}
        inline auto final_suspend() noexcept {return std::suspend_always{};}

        inline void return_value(ReturnType value) noexcept {
            this->_return_result = value;
        }

        inline std::suspend_always yield_value(embed::Suspend value) noexcept {
            this->_yield_result = value; 
            this->_is_yielding = true;
            return {};
        }

        inline void clear_yield() noexcept {
            this->_is_yielding = false;
        }

        inline bool is_yielding() const noexcept {
            return this->_is_yielding;
        }
        
        inline void unhandled_exception() noexcept {/* Panic in the Disco! */} 

        inline ReturnType get_return_result() const noexcept {return this->_return_result;}

        /// @brief returns true if the promis yielded with `co_yield`. Also clears the yield flag. So consecutive reads will return false.
        inline embed::Suspend get_yield_result() noexcept {
            this->clear_yield();
            return this->_yield_result;
        }

        inline void set_awaiting() noexcept {this->_is_awaiting=true;}
        inline void clear_awaiting() noexcept {this->_is_awaiting=false;}
        inline bool is_awaiting() const noexcept {return this->_is_awaiting;}

        // TODO: manual memory management
        // static void* operator new(std::size_t size);
        // static void operator delete(void* ptr);
    };    

}