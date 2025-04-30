#pragma once

//std
#include <coroutine>
#include <variant>
#include <memory_resource>
#include <optional>
#include <string_view>

//embed
#include <embed/Exceptions/Exceptions.hpp>
#include <embed/Containers/ArrayList.hpp>
#include <embed/OS/Exit.hpp>
#include <embed/OS/CoSignal.hpp>


namespace embed{

    // foreward declarations
    class Task;
    class AwaitableNode;
    class CoroutineNode;
    template<class ReturnType> class Coroutine;
    template<class ReturnType> class CoroutinePromise;

    inline std::pmr::memory_resource* coroutine_frame_allocator = nullptr;

    /**
     * @brief Interface tobe awaited via `co_await` and cooperativly works together will all other `embedOS` async infrastructure
     * 
     * The `AwaitableNode` is the final leaf of `embed`'s reverse linked coroutine chain. 
     * 
     * Example:
     * ```
     * send_data(tx_data);
     * rx_data = co_await receive_data();
     * ```
     * 
     * Derive from `AwaitableNode` and implement:
     * 
     * - `bool await_ready() const noexcept override { ...Code... }`
     * 
     * - `auto await_resume() const noexcept { ...Code... }`
     * 
     * optionally also define:
     * 
     * - `void await_suspend_signal(Task* task) noexcept override { ... Code ... }`
     * 
     * if you want to send a signal to the task
     */
    class AwaitableNode{
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

        /**
         * @brief Overridable: Returns `true` if the result of the awaitable is ready.
         */
        virtual bool await_ready() const noexcept = 0;

        /**
         * @brief Overridable: gets called after the Awaitable node has been registered with the master.
         * 
         * Allows to send a signal to the master
         */
        virtual CoSignal await_suspend_signal() noexcept {return CoSignal().await();}

        /** 
         * @brief Appends itself to the existing linked list of coroutines and registers itself (node) as the new leaf/tail of the list by the master (container)
         * @tparam ReturnType Generic return type for custom coroutines with custom returns
         * @param handle A handle that has to be an `embed::CoroutinePromise` but can have any return type. 
         * */ 
        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept {
            // register leaf in master to tell it what awaitable to wait for
            handle.promise().master()->register_leaf(this);

            // optionally send a signal to the master
            handle.promise().master()->signal(await_suspend_signal());
        }
    };

    /**
     * \brief Represents a coroutine (node) in a reverse-linked list of nested coroutines
     * 
     * A coroutine node is the common interface between coroutines of different return types
     * and allows arbitrary nesting of such.
     * 
     * If a new coroutine is being `co_await`ed, this class makes sure it is being registered at
     * the root Task. Further, it stores a handle to its parent/callers coroutine.
     * 
     * If the coroutine finishes, e.g.: via `co_return`, then it will un-register itself and
     * re-register the parent/callers coroutine for execution.
     * 
     * ### Mandatory Overloads
     * Overload the following methods:
     * 
     * - `void destroy() noexcept`
     * 
     * - `void resume() noexcept`
     * 
     * - `bool is_done() const noexcept`
     * 
     * 
     * @see embed::Task
     * @see embed::Coroutine
     * @see embed::CoroutinePromise
     */
    class CoroutineNode{
    private:
        Task* _master = nullptr;
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
        virtual bool is_done() const noexcept = 0;

        inline constexpr Task* master(){return this->_master;}
        inline constexpr const Task* master() const {return this->_master;}

        inline constexpr CoroutineNode* parent() {return this->_parent;}
        inline constexpr const CoroutineNode* parent() const {return this->_parent;}


        inline void Register(Task* master){this->_master = master;}

        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept;
        inline bool await_ready() const noexcept {return this->is_done();};

        inline std::suspend_always final_suspend() noexcept;
        inline std::suspend_always initial_suspend() noexcept {return std::suspend_always{};}
    };

    /**
     * \brief Owning handle to a coroutine with a custom promise type.
     *
     * This class represents a coroutine that yields a result of type `ReturnType` via `co_return`.
     * It provides mechanisms to manage coroutine execution and lifecycle, including resuming execution,
     * checking completion, and accessing the coroutine's internal promise object.
     * It wraps a coroutine handle (`std::coroutine_handle<CoroutinePromise<ReturnType>>`) and owns its and the 
     * associated promise types (`CoroutinePromise<ReturnType>`) lifetime. 
     *
     * ### Awaiting on Coroutine
     * This type implements the awaiter interface, enabling it to be `co_await`-ed inside other coroutines (nested coroutines).
     * The methods `await_ready`, `await_suspend`, and `await_resume` define the suspension behavior:
     * - `await_ready()` checks if the coroutine has already completed.
     * - `await_suspend()` suspends the caller until this coroutine completes.
     * - `await_resume()` retrieves the result from the coroutine.
     * 
     * 
     * ### Usage Example
     * ```cpp
     * Coroutine<int> MyCoroutine();
     * 
     * Coroutine<int> coro = MyCoroutine();
     * if (coro) {
     *     coro.resume();
     *     if (coro.is_done()) {
     *         int result = coro.promise().get_value(); // or coro.await_resume()
     *     }
     * }
     * ```
     * 
     * \tparam ReturnType The type returned from the coroutine using `co_return`. If `void`, the coroutine does not produce a result.
     *
     * \see embed::CoroutinePromise
     * \see std::coroutine_handle
     */
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

        inline void Register(Task* task);

        inline bool is_done() const;

        explicit inline operator bool() const noexcept;

        inline CoroutinePromise<ReturnType>& promise();

        inline CoroutineNode* node();
        inline const CoroutineNode* node() const;

        inline bool await_ready() const noexcept;
        
        template<class T>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) noexcept;

        inline ReturnType await_resume();

    };

    /**
     * \brief A `Task` (short for coroutine task) is the root of a linked list of nested coroutines.
     * 
     * A Task contains one or more (nested) coroutines in an acyclic graph. 
     * It will always resume the last/leaf coroutine and is done onece all coroutines have `co_retun`ed.
     * 
     * A Task is itsef an `AwaitableNode` and thus might be `co_await`ed by another task.
     * This might be useful, for example if one task issues 3 other tasks to the scheduler, and the `co_awaits` all of them
     * to continue its own execution only once all are finished.
     * 
     * The leaf coroutine can communicate to the Task via the interface provided by the `AwaitableNode` that 
     * is `co_await`ed. Through `co_await` the leaf coroutine can send a signal to the Task (and thus the scheduler) 
     * to tell that it is awaiting on some external task, hardware, I/O, etc.
     * 
     * You might customise the behaviour of the task by overloading the error handler,
     * that will be invoked on uncaught exceptions.
     * ```
     * virtual void handle_exception(std::exception_ptr except_ptr);
     * ```
     * 
     * \see embed::AwaitableNode
     * \see embed::LinearScheduler
     */
    class Task{
    private:
        std::string_view _task_name = "";
        Coroutine<embed::Exit> _main_coroutine;
        CoroutineNode* _leaf_coroutine;
        AwaitableNode* _leaf_awaitable = nullptr;
        CoSignal _signal; // TODO: remove from Task and add to RealTimeTask
        unsigned int _id = 0;
        bool _instant_resume = false;
        
    public:

        Task(Coroutine<embed::Exit>&& main, std::string_view task_name = "") noexcept;
        Task(const Task&)=delete;
        Task(Task&& other) noexcept;
        Task& operator=(const Task&)=delete;
        Task& operator=(Task&& other) noexcept;

        virtual ~Task(){}

        constexpr void id(unsigned int id){this->_id = id;} // TODO: make only visible for scheduler
        constexpr unsigned int id() const {return this->_id;}


        constexpr std::string_view name() const {return this->_task_name;}

        /**
         * @brief sets a signal
         */
        inline void signal(const CoSignal& signal){this->_signal = signal;}
        
        /**
         * @brief reads and clears the signal
         */
        inline const CoSignal get_signal() {
            CoSignal result = this->_signal;
            this->_signal.none(); // clear signal
            return result;
        }

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

        /**
         * @brief resumes the task/coroutine
         * 
         * This the only resume point for the whole coroutine chain. 
         * Every coroutine and nested coroutine suspends to here and will be executed/resumed from here 
         * by resuming the leaf/tail of the coroutine linked list.
         * 
         * > Note: a task needs to be resumeable, that is `is_resumeable()` returns `true` before calling this method.
         * > Otherwise an assertion is thrown at `ASSERTION_LEVEL_O1` or higher - if lower: Undefined behaviour UB ⚠.
         * 
         * @throws An AssertionFailuer if resumed while `is_resumeable()` is `false`, but only in `ASSERTION_LEVEL_O1` or higher
         */
        void resume();
        
        /// @brief returns `true` if the awaitable that this task is waiting on is ready and `.resume()` can be called again
        /// @details also returns `true` if there is currently no awaitable that is being waited on.
        bool is_resumable() const;

        /// @brief returns `true` if the awaitable that this task is waiting on is still waiting for completion - aka. this task is not resumable
        /// @details equivalent to `!task->is_resumable()`
        inline bool is_awaiting() const {return (this->_leaf_awaitable) ? !this->_leaf_awaitable->await_ready() : false;}

        /// @brief returns `true` if the main/root coroutine is done - thus the task is done
        inline bool is_done() const {return this->_main_coroutine.is_done();}

        /// @brief same as `is_done()` but for interoperability with `co_await`
        inline bool await_ready() const noexcept {return this->is_done();}

        /// @brief interoperability with `co_await` 
        inline Exit await_resume();

        inline Exit exit_status();

        /// @brief exception handler - will be called if an un-catched exception in a coroutine arises
        virtual void handle_exception(std::exception_ptr except_ptr);

        /// @brief kills the coroutine chain.
        void kill_chain();
    };

    /**
     * \brief Wraps awaitables that are not yet derived from `embed::AwaitableNode`
     * 
     * If wraps arbitrary awaitables that do not derive from `embed::AwaitableNode` that 
     * implements the registration infrastructure relied on by `embed::Coroutine` and `embed::Task`.
     * 
     * Specialises for awaitable passed as lvalues.
     * 
     * Intended to be used only by `CoroutinePromise::await_transform(awaitable)`
     * 
     * \tparam Awaitable an awaitable that implements `.await_ready()`, `.await_resume()` and `.await_suspend(handle)`.
     * 
     * \see embed::AwaitableNode
     * \see embed::Coroutine
     * \see embed::CoroutinePromise
     * \see embed::Task
     * \see embed::wrap_awaitable(Awaitable&& awaitable)
     */
    template<class Awaitable, bool is_lvalue=true>
    requires (!std::derived_from<Awaitable, AwaitableNode>)
    class AwaitableWrapper : public AwaitableNode{
    private:
        Awaitable& _awaitable;

    public:
        constexpr AwaitableWrapper(Awaitable& awaitable)
            : _awaitable(awaitable){}

        constexpr bool await_ready() const noexcept override {
            return this->_awaitable.await_ready();
        }

        constexpr auto await_resume() noexcept {
            return this->_awaitable.await_resume();
        }

        template<class Handle>
        constexpr auto await_suspend(Handle handle) noexcept {
            this->AwaitableNode::await_suspend(handle);

            // await_suspend is optional, so check if the Type supports it at compile time and call it conditionally
            if constexpr (requires {this->_awaitable.await_suspend(handle);} ){
                return this->_awaitable.await_suspend(handle);
            }else{
                return void();
            }
        }
    };

    /**
     * \brief Wraps awaitables that are not yet derived from `embed::AwaitableNode`
     * 
     * If wraps arbitrary awaitables that do not derive from `embed::AwaitableNode` that 
     * implements the registration infrastructure relied on by `embed::Coroutine` and `embed::Task`.
     * 
     * Specialises for awaitable passed as rvalues.
     * 
     * Intended to be used only by `CoroutinePromise::await_transform(awaitable)`
     * 
     * \tparam Awaitable an awaitable that implements `.await_ready()`, `.await_resume()` and `.await_suspend(handle)`.
     * 
     * \see embed::AwaitableNode
     * \see embed::Coroutine
     * \see embed::CoroutinePromise
     * \see embed::Task
     * \see embed::wrap_awaitable(Awaitable&& awaitable)
     */
    template<class Awaitable>
    requires (!std::derived_from<Awaitable, AwaitableNode>)
    class AwaitableWrapper<Awaitable, false> : public AwaitableNode{
    private:
        Awaitable _awaitable;

    public:
        constexpr AwaitableWrapper(Awaitable&& awaitable)
            : _awaitable(std::move(awaitable)){}

        constexpr bool await_ready() const noexcept override {
            return this->_awaitable.await_ready();
        }

        constexpr auto await_resume() noexcept {
            return this->_awaitable.await_resume();
        }

        template<class ReturnType>
        constexpr auto await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept {
            this->AwaitableNode::await_suspend(handle);
            return this->_awaitable.await_suspend(handle);
        }
    };

    /**
     * \brief Construction helper for AwaitableWrapper
     * 
     * `AwaitableWrapper` has two specialisations: awaitables passed as lvalue- and rvalue-references.
     * Depending on how the awaitable is passed to `wrap_awaitable()` the corresponding specialisation is instantiated.
     * 
     * \tparam Awaitable and awaitable that does not derive from `AwaitableNode`
     * \param awaitable the awaitable object as lvalue- or rvalue-reference
     * 
     * \see embed::AwaitableWrapper
     * \see embed::AwaitableNode
     */
    template<class Awaitable>
        requires (!std::derived_from<Awaitable, AwaitableNode>)
    AwaitableWrapper<Awaitable, std::is_lvalue_reference<Awaitable>::value> wrap_awaitable(Awaitable&& awaitable){
        if constexpr (std::is_lvalue_reference<Awaitable>::value){
            return AwaitableWrapper<Awaitable, true>(awaitable);
        }else{
            return AwaitableWrapper<Awaitable, false>(std::move(awaitable));
        }
    }

     /**
     * \brief Stores the Allocation size of a coroutine for inspection
     * 
     * called by the corouine promise types new operator overload by reinterpreting the allocation `void*` pointer.
     * 
     * \see embed::CoroutinePromise
     */
    struct CoroutinePromiseSize{
        std::size_t _allocated_size;

        constexpr std::size_t allocated_size() const {return this->_allocated_size;}
    };

    /**
     * \brief Promise type for an embed::Coroutine
     * 
     * This class implements the coroutine promise required by the C++20 coroutine machinery.
     * `CoroutinePromise<ReturnType>` is instantiated by the compiler when a coroutine with `Coroutine<ReturnType>` as its return type is used.
     * It inherits from `CoroutinePromiseSize` and `CoroutineNode` to integrate with the custom coroutine framework’s memory management
     * and scheduling infrastructure.
     * 
     * ### Awaitable Support
     * The `await_transform()` method is used to wrap arbitrary awaitables unless they already derive from a framework-specific `AwaitableNode`.
     * This supports customization of how `co_await` expressions behave within the coroutine body.
     * 
     * ### Memory Management
     * Overrides `operator new` and `operator delete` to allocate memory for the coroutine frame from a custom allocator (`coroutine_frame_allocator`).
     * This supports embedded or specialized memory management scenarios.
     * 
     * \see embed::Coroutine
     * \see embed::CoroutinePromiseSize
     * \see embed::CoroutineNode
     * \see embed::AwaitableWrapper
     */
    template<class ReturnType>
    class CoroutinePromise : public CoroutinePromiseSize, public CoroutineNode{
    private:
        ReturnType _return_value;
    
    public:

        template<class Awaitable>
        inline auto await_transform(Awaitable&& awaitable){
            if constexpr (!std::derived_from<Awaitable, AwaitableNode>){
                return wrap_awaitable(std::forward<Awaitable>(awaitable));
            }else{
                return std::forward<Awaitable>(awaitable);
            }
        }

        inline auto get_return_object(){return Coroutine<ReturnType>(std::coroutine_handle<CoroutinePromise>::from_promise(*this));}
        

        /// @brief Print an error message and kill the task
        inline void unhandled_exception() noexcept {
            this->master()->handle_exception(std::current_exception());
        } 

        inline void return_value(const ReturnType& value){this->_return_value = value;}
        inline void return_value(ReturnType&& value){this->_return_value = std::move(value);}

        inline ReturnType&& get_return_value() {
            EMBED_ASSERT_O1_MSG(this->is_done(), "Tried to get coroutine return value, before `is_done()`. S: Read value after the coroutine is finished. Check for `is_done()` or use co_await");
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
        
        inline bool is_done() const noexcept  override{
            // use const cast, because the `std::coroutine_handle<>::from_promise()` has no `const` version but `is_done()` is still `const`, so it it fine here.
            // `const_cast` is not ideal but necessary due to the lack of `const` overloads of `std::coroutine_handle<>::from_promise()`
            return std::coroutine_handle<CoroutinePromise>::from_promise(const_cast<CoroutinePromise&>(*this)).done();
        }
        
        inline bool await_ready() const noexcept {return this->is_done();}

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
    inline void Coroutine<T>::Register(Task* task){this->coro.promise().Register(task);}

    template<class T>
    inline bool Coroutine<T>::is_done() const {return this->coro.done();}

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
//                                                   Task: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    inline Exit Task::await_resume(){
        EMBED_ASSERT_O1(this->is_done());
        return this->_main_coroutine.promise().get_return_value();
    }
    
    inline Exit Task::exit_status() {
        EMBED_ASSERT_O1(this->is_done());
        return this->_main_coroutine.promise().get_return_value();
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
        if(this->_parent != nullptr){
            this->_master->register_leaf(this->_parent);
        }
        return std::suspend_always{};
    }

}