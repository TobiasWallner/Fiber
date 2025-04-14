#pragma once

//std
#include <coroutine>
#include <variant>
#include <memory_resource>

//embed
#include "embed/Exceptions/Exceptions.hpp"
#include <embed/OS/Exit.hpp>
#include "embed/Containers/StaticArrayList.hpp"

/**
 * @page coroutine_execution_model Coroutine Execution Model
 *
 * This page describes the core architecture behind coroutine-based task scheduling in embedOS.
 * The coroutine system is designed for real-time embedded environments, supporting deeply nested coroutine chains
 * with strict memory control, deterministic behavior, and minimal runtime overhead.
 *
 * ---
 *
 * ## Overview
 *
 * embedOS uses C++20 coroutines to model cooperative tasks. Each task consists of one or more coroutines
 * connected via `co_await` chains. These coroutines form a **reverse-linked list**, where:
 *
 * - Each coroutine node holds a pointer to its **parent coroutine**
 * - The **tail coroutine** is the currently active/resumable node
 * - The **head coroutine** is the original entry point (`Task::main()` or similar)
 *
 * The list structure supports:
 * - Efficient resumption from a single point
 * - Clear, centralized exception and failure handling
 * - Manual memory control and cleanup
 *
 * ---
 *
 * ## Roles
 *
 * - **Coroutine**:
 *   - Represents a single coroutine frame (created from `co_await`, `co_yield`, or `co_return`)
 *   - A coroutine may await another coroutine, forming a parent-child link
 *   - Coroutines are nodes in the reverse-linked list, but not always at the tail
 *
 * - **Awaitable**:
 *   - A special form of coroutine node that is always the **leaf node**
 *   - For example: `co_await delay(10ms)` or `co_await future`
 *
 * - **CoTask**:
 *   - Owns the coroutine chain
 *   - Holds:
 *     - A pointer to the **root** (head/original coroutine)
 *     - A pointer to the **leaf** (tail/current active coroutine)
 *   - Responsible for:
 *     - Resuming the task (`resume()` calls the current leaf coroutine)
 *     - Handling exceptions
 *     - Destroying the coroutine chain
 *     - Tracking lifecycle and status
 * 
 * This means that this coroutine architecture has a memory complexity of O[1] - stack depth does not grow with the number or nested coroutines.
 * Coroutines are only logically nested but physically flat.
 *
 * ---
 *
 * ## Execution Flow
 *
 * 1. `CoTask::resume()` is called
 * 2. It resumes the **leaf coroutine** in the chain
 * 3. The coroutine may:
 *     - `co_await` another coroutine → creates a new coroutine and sets it as the new leaf
 *     - `co_return` or `co_yield` → returns to `final_suspend()`
 * 4. After `final_suspend()`, control returns to `CoTask`
 * 5. If there is a parent coroutine, `CoTask` registers that as the new leaf and resumes it immediately
 * 6. If the chain completes, `CoTask` marks the task as `Exit::Success`
 *
 * All control flow and state transition pass through `CoTask`. The coroutine chain **never resumes itself**.
 *
 * ---
 *
 * ## Exception Handling
 *
 * - When a coroutine throws and does not catch the exception:
 *   - The compiler-generated `try/catch` calls `promise_type::unhandled_exception()`
 *   - That stores `std::current_exception()` inside the coroutine's promise
 *   - Then calls `CoTask::handle_exception()`
 *
 * - `CoTask::handle_exception()`:
 *   - Re-throws the exception to identify its type
 *   - Logs diagnostic output
 *   - Calls `kill_chain()` to destroy all coroutine frames (from leaf to root)
 *   - Marks the task as `Exit::Failure`
 *
 * ---
 *
 * ## Coroutine Chain Destruction
 *
 * - Coroutine memory is not automatically freed unless `.destroy()` is called
 * - `CoTask::kill_chain()` iterates from the **leaf up to the root**, calling `.destroy()` on each coroutine frame
 * - This minimizes stack usage and avoids leaks or dangling handles
 *
 * ---
 *
 * ## Core Invariants
 *
 * - Only `CoTask` is allowed to resume coroutines
 * - Every `co_await`, `co_yield`, and `co_return` returns control to `CoTask`
 * - No coroutine resumes another coroutine directly
 * - The active coroutine is always the tail of the chain (`leaf`)
 * - The chain is always linear and acyclic
 * - All coroutine frames are cleaned up either on success or failure
 *
 * ---
 *
 * ## Analogy
 *
 * Think of the coroutine chain as a linked list:
 *
 * - `Coroutine` is a **node**
 * - `CoTask` is the **owner and controller** of the list
 * - `co_await` links nodes in **reverse**, so the list grows toward the leaf
 * - Only `CoTask` traverses and manipulates the list
 *
 * ---
 *
 * ## Notes
 *
 * - The entire system works without heap allocation if a static memory allocator is used
 * - All coroutines inherit from `CoroutineNode` and register themselves with their task
 * - `CoroutineNode::final_suspend()` bubbles the parent coroutine handle back to the task
 * - Coroutines and tasks use ANSI-formatted logs and structured assertions for diagnostics
 *
 * ---
 *
 * @see embed::Coroutine
 * @see embed::CoTask
 * @see embed::CoroutineNode
 * @see embed::CoroutinePromise
 * @see embed::Future
 * @see embed::Promise
 */

namespace embed{

    // foreward declarations
    class CoTask;
    class AwaitableNode;
    class CoroutineNode;
    template<class ReturnType> class Coroutine;
    template<class ReturnType> class CoroutinePromise;


    inline std::pmr::memory_resource* coroutine_frame_allocator = nullptr;

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

    class CoroutineNode{
    private:
        CoTask* _master = nullptr;
        CoroutineNode* _parent = nullptr;

    public: 
        virtual ~CoroutineNode() noexcept = default;

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
        
        virtual void destroy() noexcept = 0;
        virtual void resume() noexcept = 0;
        virtual bool done() const noexcept = 0;

        inline constexpr CoTask* master(){return this->_master;}
        inline constexpr const CoTask* master() const {return this->_master;}

        inline constexpr CoroutineNode* parent() {return this->_parent;}
        inline constexpr const CoroutineNode* parent() const {return this->_parent;}


        inline void Register(CoTask* master){this->_master = master;}

        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept;
        inline bool await_ready() const noexcept {return this->done();};

        inline std::suspend_always final_suspend() noexcept;
        inline std::suspend_always initial_suspend() noexcept {return std::suspend_always{};}
    };


    //TODO: Migrate CoroutineNode from Coroutine to CoroutinePromise
    template<class ReturnType>
    class Coroutine {
    public:
        using promise_type = CoroutinePromise<ReturnType>;
        using promise_handle = std::coroutine_handle<promise_type>;
        promise_handle coro = nullptr;
    public:

        inline Coroutine() = default;
        explicit Coroutine(promise_handle&& h) : coro(std::move(h)) {}

        inline Coroutine(const Coroutine&) = delete;
        inline Coroutine& operator=(const Coroutine&) = delete;
        inline Coroutine(Coroutine&& other) noexcept : coro(std::move(other.coro)) {
            other.coro = nullptr;
        }
        inline Coroutine& operator=(Coroutine&& other) noexcept {
            if (this != &other) {
                this->coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }

        inline ~Coroutine() noexcept;

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

        inline void resume();

        inline void destroy();

        inline void Register(CoTask* task);

        inline bool done() const;

        explicit inline operator bool() const noexcept;

        inline CoroutinePromise<ReturnType>& promise();

        inline CoroutineNode* node();
        inline const CoroutineNode* node() const;

        inline bool await_ready() const noexcept;
        
        template<class T>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) noexcept;

        inline ReturnType await_resume();

    };


    class CoTask : public AwaitableNode{
    private:
        const char * _task_name = "";
        Coroutine<embed::Exit> _main_coroutine;
        CoroutineNode* _leaf_coroutine;
        AwaitableNode* _leaf_awaitable = nullptr;
        std::size_t _task_id = 0;
        bool _instant_resume = false;
    public:

        CoTask(const char* task_name, Coroutine<embed::Exit>&& main) noexcept;
        CoTask(const CoTask&)=delete;
        CoTask(CoTask&& other) noexcept;
        CoTask& operator=(const CoTask&)=delete;
        CoTask& operator=(CoTask&& other) noexcept;

        inline ~CoTask(){}

        inline void destroy(){this->_main_coroutine.destroy();}

        /// @brief Registers the leaf nested coroutine that serves as the resume point after suspensions
        /// @details Meant to be called by the Coroutine
        /// @param leaf a coroutine handle to the leaf tasks
        inline void register_leaf(CoroutineNode* leaf) noexcept {
            if(this->_leaf_coroutine){
                this->_leaf_coroutine = leaf;
                this->_instant_resume = true;
            }
        }

        /// @brief Registers a leaf awaitable that will be waited on before resuming its parents coroutine
        /// @details Meant to be called by the Awaitable
        /// @param awaitable The awaitable that will be waited on
        inline void register_leaf(AwaitableNode* awaitable) noexcept {this->_leaf_awaitable = awaitable;}

        /// @brief Safely resumes the task/coroutine
        /// @details This the only resume point for the whole coroutine chain. Every coroutine and nested coroutine suspends to here and will be executed/resumed from here by resuming the leaf/tail of the coroutine linked list.
        /// @return `true` if the coroutine was resumed, `fals` if it is still suspended and waiting on `await_ready()` 
        bool resume();
        
        /// @brief returns `true` if the awaitable that this task is waiting on is ready and `.resume()` can be called again
        /// @details also returns `true` if there is currently no awaitable that is being waited on.
        bool is_resumable() const;

        /// @brief returns `true` if the awaitable that this task is waiting on is still waiting for completion - aka. this task is not resumable
        /// @details equivalent to `!task->is_resumable()`
        inline bool is_awaiting() const {return (this->_leaf_awaitable) ? !this->_leaf_awaitable->await_ready() : false;}

        /// @brief returns `true` if the main/root coroutine is done - thus the task is done
        inline bool done() const {return this->_main_coroutine.done();}

        /// @brief same as `done()` but for interoperability with `co_await`
        inline bool await_ready() const noexcept override {return this->done();}

        inline Exit await_resume();

        inline Exit exit_status();

        /// @brief exception handler - will be called if an un-catched exception in a coroutine arises
        virtual void handle_exception(std::exception_ptr except_ptr);

        /// @brief kills the coroutine chain.
        void kill_chain();
    };

    struct CoroutinePromiseSize{
            std::size_t _allocated_size;

            constexpr std::size_t allocated_size() const {return this->_allocated_size;}
    };

    template<class ReturnType>
    struct CoroutinePromise : public CoroutinePromiseSize, public CoroutineNode{
    private:
        ReturnType _return_value;
        
    
    public:
        inline auto get_return_object(){return Coroutine<ReturnType>(std::coroutine_handle<CoroutinePromise>::from_promise(*this));}
        

        /// @brief Print an error message and kill the task
        inline void unhandled_exception() noexcept {
            this->master()->handle_exception(std::current_exception());
        } 

        inline void return_value(const ReturnType& value){this->_return_value = value;}
        inline void return_value(ReturnType&& value){this->_return_value = std::move(value);}

        inline ReturnType&& get_return_value() {
            EMBED_ASSERT_O1_MSG(this->done(), "Tried to get coroutine return value, before `done()`. S: Read value after the coroutine is finished. Check for `done()` or use co_await");
            return std::move(_return_value);
        }

        inline static void* operator new(std::size_t size){
            EMBED_ASSERT_CRITICAL_MSG(coroutine_frame_allocator != nullptr, "No memory resource provided. S: Assign a memory `embed::coroutine_mem_resource= &resource;`");
            void* result = coroutine_frame_allocator->allocate(size);
            reinterpret_cast<CoroutinePromiseSize*>(result)->_allocated_size = size;
            return result;
        }

        inline static void operator delete(void* ptr, std::size_t size){
            coroutine_frame_allocator->deallocate(ptr, size);
            return;
        }
        
        inline void destroy() noexcept override{
            std::coroutine_handle<CoroutinePromise>::from_promise(*this).destroy();
        }
        
        inline void resume() noexcept override{
            std::coroutine_handle<CoroutinePromise>::from_promise(*this).resume();
        }
        
        inline bool done() const noexcept  override{
            // use const cast, because the `std::coroutine_handle<>::from_promise()` has no `const` version but `done()` is still `const`, so it it fine here.
            // `const_cast` is not ideal but necessary due to the lack of `const` overloads of `std::coroutine_handle<>::from_promise()`
            return std::coroutine_handle<CoroutinePromise>::from_promise(const_cast<CoroutinePromise&>(*this)).done();
        }
        
        inline bool await_ready() const noexcept {return this->done();}

        inline ReturnType await_resume() {
            EMBED_ASSERT_CRITICAL_MSG(this->await_ready(), "Resume on unready coroutnie awaitable. S: Check `await_ready()` before calling `resume()`.");
            return std::move(this->_return_value); // move out before destroying to prevent use after free
        }
    };

    
//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   Coroutine: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    template<class T>
    inline Coroutine<T>::~Coroutine() noexcept {
        if(this->coro != nullptr){
            this->coro.destroy();
            this->coro = nullptr;
        } 
    }

    template<class T>
    inline void Coroutine<T>::resume() {this->coro.resume();}

    template<class T>
    inline void Coroutine<T>::destroy() {
        if(this->coro != nullptr) this->coro.destroy(); // prevent accidental double free
    }

    template<class T>
    inline void Coroutine<T>::Register(CoTask* task){this->coro.promise().Register(task);}

    template<class T>
    inline bool Coroutine<T>::done() const {return this->coro.done();}

    template<class T>
    inline Coroutine<T>::operator bool() const noexcept {return this->coro != nullptr;}

    template<class T>
    inline CoroutinePromise<T>& Coroutine<T>::promise() {return this->coro.promise();}

    template<class T>
    inline CoroutineNode* Coroutine<T>::node() {return &this->coro.promise();}

    template<class T>
    inline const CoroutineNode* Coroutine<T>::node() const {return &this->coro.promise();}

    template<class T>
    inline bool Coroutine<T>::await_ready() const noexcept {
        return this->coro.promise().await_ready();
    }
    
    template<class ReturnValue>
    template<class T>
    inline void Coroutine<ReturnValue>::await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) noexcept {
        this->coro.promise().await_suspend(handle);
    }

    template<class T>
    inline T Coroutine<T>::await_resume() {
        EMBED_ASSERT_CRITICAL_MSG(this->coro, "Attempting to read from a deleted coroutine. S: Remove double `await_resume()`. Check for call to `await_resume()` after `co_await`.");
        return this->coro.promise().await_resume();
    }
    
//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   CoTask: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    inline Exit CoTask::await_resume(){
        EMBED_ASSERT_O1(this->done());
        return this->_main_coroutine.promise().get_return_value();
    }
    
    inline Exit CoTask::exit_status() {
        EMBED_ASSERT_O1(this->done());
        return this->_main_coroutine.promise().get_return_value();
    }

//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   AwaitableNode: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------


    /// @brief Appends itself to the existing linked list of coroutines and registers itself (node) as the new leaf/tail of the list by the master (container)
    /// @tparam ReturnType Generic return type for custom coroutines with custom returns
    /// @param handle A handle that has to be an `embed::CoroutinePromise` but can have any return type. 
    template<class ReturnType>
    inline void AwaitableNode::await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept {
        // store the master
        this->master = handle.promise().future().master;

        // register leaf in master to tell it what awaitable to wait for
        this->master->register_leaf(this);
    }

//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   CoroutineNode: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------


    template<class ReturnType>
    inline void CoroutineNode::await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept {
        // store parent and master
        this->_parent = &handle.promise();
        this->_master = handle.promise().master();

        // register leafe in master to tell it what coroutine to resume next
        this->_master->register_leaf(this);
    }

    inline std::suspend_always CoroutineNode::final_suspend() noexcept {
        // unregister self and re-register the parent, so next time the master will resume the parent
        if(this->_parent){
            this->_master->register_leaf(this->_parent);
        }
        return std::suspend_always{};
    }

}