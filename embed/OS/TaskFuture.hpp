#pragma once

//std
#include <coroutine>
#include <memory_resource>
		

//embed

#include "embed/Exceptions.hpp"
#include "embed/OStream.hpp"

#include "embed/OS/Exit.hpp"
#include "embed/OS/Yield.hpp"
#include "embed/OS/Cycle.hpp"

namespace embed{

    /// @brief pointer to a standard memory ressource that will be used to allocate the coroutine frames
    inline std::pmr::memory_resource* coroutine_frame_allocator = nullptr;

    // foreward declaration
    template<class ReturnType>
    class TaskPromise;

    enum class CoroutineStatusType{
        Ready,
        Yield,
        Cycle,
        Await,
        Return,
        Delay
    };

    inline OStream& operator<<(OStream& stream, CoroutineStatusType status){
        #define EMBED_CASE(value) case CoroutineStatusType::value: return stream << FormatStr(#value, sizeof(#value))
        switch(status){
            EMBED_CASE(Ready);
            EMBED_CASE(Yield);
            EMBED_CASE(Cycle);
            EMBED_CASE(Await);
            EMBED_CASE(Return);
            EMBED_CASE(Delay);
            default : return stream << FormatStr("N/A", 3);
        };
        #undef EMBED_CASE
    }

    template<class ReturnType>
    struct TaskFuture{
        using promise_type = TaskPromise<ReturnType>;
        using handle_type = std::coroutine_handle<promise_type>;

    private:
        handle_type coro = nullptr;

    public:

        /// @brief default constructor 
        inline TaskFuture() = default;

        /// @brief construct forom coroutine handle
        /// @param h the coroutine handle
        explicit TaskFuture(handle_type h) : coro(h) {}

        /// @brief deleted copy constructor 
        inline TaskFuture(const TaskFuture&) = delete;

        /// @brief deleted copy assignment 
        inline TaskFuture& operator=(const TaskFuture&) = delete;

        /// @brief is move constructible
        inline TaskFuture(TaskFuture&& other) noexcept : coro(other.coro) {
            other.coro = nullptr;
        }

        /// @brief is move assignable
        inline TaskFuture& operator=(TaskFuture&& other) noexcept {
            if (this != &other) {
                this->coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }

        /// @brief destructor
        inline ~TaskFuture() {if (this->coro) this->coro.destroy();}

        /// @brief resumes the coroutine
        inline void resume() {
            if (this->coro){ 
                if(!this->is_awaiting()){
                    this->coro.promise()._return_type = CoroutineStatusType::Yield; // set to yield in case of unsupported use of awaitables
                    this->coro.resume();
                }
            }
        }

        /// @brief checks if the coroutine is done
        /// @return returns `true` if the coroutine is done
        inline bool done() const {return this->coro.done();}

        inline bool is_awaiting() const {return this->coro.promise().is_awaiting();}
        inline bool is_yielding() const {return this->coro.promise().is_yielding();}
        inline bool is_ending_cycle() const {return this->coro.promise().is_ending_cycle();}
        inline bool is_returning() const {return this->coro.promise().is_returning();}
        inline bool is_delaying() const {return this->coro.promise().is_delaying();}
        inline CoroutineStatusType co_return_type() const {return this->coro.promise().co_return_type();}

        inline ReturnType get_return_value() const {
            EMBED_ASSERT_O1(this->is_returning());
            return this->coro.promise().get_return_value();
        }

        inline std::chrono::nanoseconds get_delay_value() const {
            EMBED_ASSERT_O1(this->is_delaying());
            return this->coro.promise().get_delay_value();
        }

        /// @brief checks if the coroutine exists
        /// @returns `true` if the coroutine exists and `falst` otherwise (you would have to create the coroutine first then)
        explicit inline operator bool() const noexcept {return this->coro != nullptr;}

    };

    /**
     * @brief promise for the result of a `co_await`, `co_Suspend` or `co_return` in the coroutine function (`main`) of a Task.
     * 
     * Usage in a task function:
     * ```
     * auto async_value = dma_copy(...);
     * calculate();
     * co_await async_value; // Suspends execution - equivalent to `co_Suspend ESuspend::SuspendType;`
     * while(...){
     *      calculate();
     *      co_Suspend SuspendType::SuspendType; // manually SuspendType execution to other tasks
     * }
     * co_Suspend SuspendType::Cycle; // manually suspend until the next cycle
     * calculate();
     * co_return Exit::Success; // task finished and will be removed from the scheduling queues
     * ```
     */
    template<class ReturnType>
    struct TaskPromise{
        union{
            ReturnType Return;
            std::chrono::nanoseconds delay;
        }_result;

        CoroutineStatusType _return_type = CoroutineStatusType::Ready;


        inline static void* operator new(std::size_t size){
            // TODO: call to global embed::TaskFramesAllocator
            EMBED_ASSERT_CRITICAL_MSG(coroutine_frame_allocator != nullptr, "No memory resource provided. S: Assign a memory `embed::coroutine_mem_resource= &resource;`");
            return coroutine_frame_allocator->allocate(size);
        }

        inline static void operator delete(void* ptr, std::size_t size){
            // TODO: call to global embed::TaskFramesAllocator
            EMBED_ASSERT_CRITICAL_MSG(coroutine_frame_allocator != nullptr, "No memory resource provided. S: Assign a memory `embed::coroutine_mem_resource= &resource;`");
            coroutine_frame_allocator->deallocate(ptr, size);
            return;
        }

        inline auto get_return_object(){return TaskFuture<ReturnType>{std::coroutine_handle<TaskPromise<ReturnType>>::from_promise(*this)};}
        inline auto initial_suspend() noexcept {return std::suspend_always{};}
        inline auto final_suspend() noexcept {return std::suspend_always{};}

        inline void return_value(ReturnType value) noexcept {
            this->_result.Return = value;
            this->_return_type = CoroutineStatusType::Return;
        }

        inline std::suspend_always yield_value(embed::Yield value) noexcept {
            this->_return_type = CoroutineStatusType::Yield;
            return {};
        }

        inline std::suspend_always yield_value(embed::Cycle value) noexcept {
            this->_return_type = CoroutineStatusType::Cycle;
            return {};
        }

        template<class Rep, class Period = std::ratio<1>>
        inline std::suspend_always yield_value(std::chrono::duration<Rep, Period> delay) noexcept {
            this->_result.delay = std::chrono::duration_cast<std::chrono::nanoseconds>(delay);
            this->_return_type = CoroutineStatusType::Delay;
            return {};
        }

        inline void unhandled_exception() noexcept {/* Panic in the Disco! */} 

        inline CoroutineStatusType co_return_type() const noexcept {return this->_return_type;}

        
        /// @brief returns `true` if the co-routine returned via `co_Suspend` and then clears that status symbol - consecutive calls will return `false`
        inline bool is_yielding() const noexcept {return this->_return_type == CoroutineStatusType::Yield;}
        inline bool is_ending_cycle() const noexcept {return this->_return_type == CoroutineStatusType::Cycle;}
        inline bool is_returning() const noexcept {return this->_return_type == CoroutineStatusType::Return;}
        inline bool is_delaying() const noexcept {return this->_return_type == CoroutineStatusType::Delay;}

        inline std::chrono::nanoseconds get_delay_value() const {
            EMBED_ASSERT_O1(this->is_delaying());
            return this->_result.delay;
        }

        inline ReturnType get_return_value() const {
            EMBED_ASSERT_O1(this->is_returning());
            return this->_result.Return;
        }

        inline void set_awaiting() noexcept {this->_return_type = CoroutineStatusType::Await;}
        inline void clear_awaiting() noexcept {this->_return_type = CoroutineStatusType::Ready;}
        inline bool is_awaiting() const noexcept {return this->_return_type == CoroutineStatusType::Await;}

        
    };    

}