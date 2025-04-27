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
#include <embed/OS/CoTaskSignal.hpp>


namespace embed{

    // foreward declarations
    class CoTask;
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
     * - `auto await_resume() noexcept { ...Code... }`
     * 
     * optionally also define:
     * 
     * - `void await_suspend_signal(CoTask* task) noexcept override { ... Code ... }`
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
        virtual CoTaskSignal await_suspend_signal() noexcept {return CoTaskSignal().await();}

        /// @brief Appends itself to the existing linked list of coroutines and registers itself (node) as the new leaf/tail of the list by the master (container)
        /// @tparam ReturnType Generic return type for custom coroutines with custom returns
        /// @param handle A handle that has to be an `embed::CoroutinePromise` but can have any return type. 
        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<embed::CoroutinePromise<ReturnType>> handle) noexcept;
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
        virtual bool is_done() const noexcept = 0;

        inline constexpr CoTask* master(){return this->_master;}
        inline constexpr const CoTask* master() const {return this->_master;}

        inline constexpr CoroutineNode* parent() {return this->_parent;}
        inline constexpr const CoroutineNode* parent() const {return this->_parent;}


        inline void Register(CoTask* master){this->_master = master;}

        template<class ReturnType>
        inline void await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept;
        inline bool await_ready() const noexcept {return this->is_done();};

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

    class CoTask : public AwaitableNode{
    private:
        std::string_view _task_name = "";
        Coroutine<embed::Exit> _main_coroutine;
        CoroutineNode* _leaf_coroutine;
        AwaitableNode* _leaf_awaitable = nullptr;
        CoTaskSignal _signal;
        unsigned int _id = 0;
        bool _instant_resume = false;
        
    public:

        CoTask(Coroutine<embed::Exit>&& main, const char* task_name = "") noexcept;
        CoTask(const CoTask&)=delete;
        CoTask(CoTask&& other) noexcept;
        CoTask& operator=(const CoTask&)=delete;
        CoTask& operator=(CoTask&& other) noexcept;

        virtual ~CoTask(){}

        constexpr void id(unsigned int id){this->_id = id;} // TODO: make only visible for scheduler
        constexpr unsigned int id() const {return this->_id;}


        constexpr std::string_view name() const {return this->_task_name;}

        /**
         * @brief sets a signal
         */
        inline void signal(const CoTaskSignal& signal){this->_signal = signal;}
        
        /**
         * @brief reads and clears the signal
         */
        inline const CoTaskSignal get_signal() {
            CoTaskSignal result = this->_signal;
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
        inline bool await_ready() const noexcept override {return this->is_done();}

        inline Exit await_resume();

        inline Exit exit_status();

        /// @brief exception handler - will be called if an un-catched exception in a coroutine arises
        virtual void handle_exception(std::exception_ptr except_ptr);

        void kill(){

        }

        /// @brief kills the coroutine chain.
        void kill_chain();
    };

    struct CoroutinePromiseSize{
            std::size_t _allocated_size;

            constexpr std::size_t allocated_size() const {return this->_allocated_size;}
    };

    template<class ReturnType>
    class CoroutinePromise : public CoroutinePromiseSize, public CoroutineNode{
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
    inline void Coroutine<T>::Register(CoTask* task){this->coro.promise().Register(task);}

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
//                                                   CoTask: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    inline Exit CoTask::await_resume(){
        EMBED_ASSERT_O1(this->is_done());
        return this->_main_coroutine.promise().get_return_value();
    }
    
    inline Exit CoTask::exit_status() {
        EMBED_ASSERT_O1(this->is_done());
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
        // register leaf in master to tell it what awaitable to wait for
        handle.promise().master()->register_leaf(this);

        // optionally send a signal to the master
        handle.promise().master()->signal(await_suspend_signal());
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