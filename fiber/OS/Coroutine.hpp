#pragma once

//std
#include <coroutine>
#include <variant>
#include <memory_resource>
#include <optional>
#include <string_view>
#include <utility>

//fiber
#include <fiber/Exceptions/Exceptions.hpp>
#include <fiber/OS/Exit.hpp>
#include <fiber/Memory/StackAllocator.hpp>
#include <fiber/OS/CoSignal.hpp>
#include <fiber/Chrono/TimePoint.hpp>


namespace fiber{

    // foreward declarations
    class Delay;
    class NextCycle;
    class TaskBase;
    struct CoroutineNode;
    template<class ReturnType> class Coroutine;
    template<class ReturnType> class CoroutinePromise;

    namespace detail{
        inline fiber::StackAllocatorExtern* frame_allocator = nullptr;
    }

    /**
     * \brief Represents a coroutine (node) in a reverse-linked list of nested coroutines
     * 
     * A coroutine node is the common interface between coroutines of different return types
     * and allows arbitrary nesting of such.
     * 
     * If a new coroutine is being `co_await`ed, this class makes sure it is being registered at
     * the root TaskBase. Further, it stores a handle to its parent/callers coroutine.
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
     * @see fiber::TaskBase
     * @see fiber::Coroutine
     * @see fiber::CoroutinePromise
     */
    struct CoroutineNode{
        std::coroutine_handle<> _handle = nullptr;
        TaskBase* _task = nullptr;
        CoroutineNode* _parent = nullptr;
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
        
        inline void destroy() noexcept;
        inline void resume() noexcept {this->_handle.resume();}
        inline bool is_done() const noexcept {return this->_handle.done();}
        constexpr operator bool(){return bool(this->_handle);}

        constexpr TaskBase* task(){return this->_task;}
        constexpr const TaskBase* task() const {return this->_task;}

        constexpr CoroutineNode* parent() {return this->_parent;}
        constexpr const CoroutineNode* parent() const {return this->_parent;}

        constexpr std::coroutine_handle<> handle() {return this->_handle;}
        constexpr const std::coroutine_handle<> handle() const {return this->_handle;}

        constexpr void Register(TaskBase* task){this->_task = task;}

        template<class ReturnType>
        constexpr void await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept;
        inline bool await_ready() const noexcept {return this->is_done();};

        constexpr std::suspend_always final_suspend() noexcept;
        constexpr std::suspend_always initial_suspend() noexcept {return std::suspend_always{};}
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
     * \see fiber::CoroutinePromise
     * \see std::coroutine_handle
     */
    template<class ReturnType=void>
    class Coroutine {
        public:
        using promise_type = CoroutinePromise<ReturnType>;
        using promise_handle = std::coroutine_handle<promise_type>;
        promise_handle coro = nullptr;
    public:

        constexpr Coroutine() = default;
        constexpr explicit Coroutine(promise_handle&& h) : coro(std::move(h)) {}

        constexpr Coroutine(const Coroutine&) = delete;
        constexpr Coroutine& operator=(const Coroutine&) = delete;
        constexpr Coroutine(Coroutine&& other) noexcept : coro(std::move(other.coro)) {
            other.coro = nullptr;
        }
        constexpr Coroutine& operator=(Coroutine&& other) noexcept {
            if (this != &other) {
                this->coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }

        constexpr ~Coroutine() noexcept;

        constexpr void resume();

        constexpr void destroy();

        constexpr void Register(TaskBase* task);

        constexpr bool is_done() const;

        explicit constexpr operator bool() const noexcept;

        constexpr CoroutinePromise<ReturnType>& promise();

        constexpr CoroutineNode* node();
        constexpr const CoroutineNode* node() const;

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

        constexpr bool await_ready() const noexcept;
        
        template<class T>
        constexpr void await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) noexcept;

        constexpr ReturnType await_resume();
    };

    template<class T>
    struct is_coroutine : public std::false_type{};

    template<class T>
    struct is_coroutine<Coroutine<T>> : public std::true_type{};

    template<class T>
    constexpr bool is_coroutine_v = is_coroutine<T>::value;

    struct Schedule{
        TimePoint ready;
        TimePoint deadline;
    };

    struct ExecutionTime{
        TimePoint start;
        TimePoint end;
    };

    /**
     * \brief A `TaskBase` (short for coroutine task) is the root of a linked list of nested coroutines.
     * 
     * A TaskBase contains one or more (nested) coroutines in an acyclic graph. 
     * It will always resume the last/leaf coroutine and is done onece all coroutines have `co_retun`ed.
     * 
     * A TaskBase is itsef an `AwaitableNode` and thus might be `co_await`ed by another task.
     * This might be useful, for example if one task issues 3 other tasks to the scheduler, and the `co_awaits` all of them
     * to continue its own execution only once all are finished.
     * 
     * The leaf coroutine can communicate to the TaskBase via the interface provided by the `AwaitableNode` that 
     * is `co_await`ed. Through `co_await` the leaf coroutine can send a signal to the TaskBase (and thus the scheduler) 
     * to tell that it is awaiting on some external task, hardware, I/O, etc.
     * 
     * You might customise the behaviour of the task by overloading the error handler,
     * that will be invoked on uncaught exceptions.
     * ```
     * virtual void handle_exception(std::exception_ptr except_ptr);
     * ```
     * 
     * \see fiber::AwaitableNode
     * \see fiber::LinearScheduler
     * 
     * \tparam N The number of bytes used for the tasks frame allocator
     */
    class TaskBase{
    public:
        std::string_view _task_name = "";
        fiber::StackAllocatorExtern* _frame_allocator;
        Coroutine<fiber::Exit> _main_coroutine;
        CoroutineNode* _leaf_coroutine = nullptr;
        bool (*_leaf_awaitable_ready_func)(const void* _leaf_awaitable_obj) = nullptr;
        const void* _leaf_awaitable_obj = nullptr;
        CoSignal _signal;
        
        uint32_t _priority = 0; // higher number = higher priority
        Schedule _schedule;
        TimePoint _execution_start;
        
        uint16_t _id = 0;

        bool _instant_resume = false;
        bool _immediatelly_ready = false; // if true, ignores `_ready_time` when entering the scheduler

        static constexpr uint32_t _deadline_priority = std::numeric_limits<uint32_t>::max();
    public:

        constexpr TaskBase() = default;
        constexpr TaskBase(const TaskBase&)=delete;
        constexpr TaskBase& operator=(const TaskBase&)=delete;

        constexpr TaskBase(TaskBase&& other) noexcept
            : _task_name(other._task_name)
            , _frame_allocator(other._frame_allocator)
            , _main_coroutine(std::move(other._main_coroutine))
            , _leaf_coroutine(other._leaf_coroutine)
            , _leaf_awaitable_ready_func(other._leaf_awaitable_ready_func)
            , _leaf_awaitable_obj(other._leaf_awaitable_obj)
            , _signal(other._signal)
            , _priority(other._priority)
            , _schedule(other._schedule)
            , _execution_start(other._execution_start)
            , _id(other._id)
            , _instant_resume(other._instant_resume)
            , _immediatelly_ready(other._immediatelly_ready)
        {
            this->_main_coroutine.Register(this); // re-register
        }

        inline TaskBase& operator=(TaskBase&& other) noexcept {
            if(this != &other){
                this->_task_name = other._task_name;
                this->_frame_allocator = other._frame_allocator;
                this->_main_coroutine = std::move(other._main_coroutine);
                this->_leaf_coroutine = other._leaf_coroutine;
                this->_leaf_awaitable_ready_func = other._leaf_awaitable_ready_func;
                this->_leaf_awaitable_obj = other._leaf_awaitable_obj;
                this->_signal = other._signal;
                this->_priority = other._priority;
                this->_schedule = other._schedule;
                this->_execution_start = other._execution_start;
                this->_id = other._id;
                this->_instant_resume = other._instant_resume;
                this->_immediatelly_ready = other._immediatelly_ready;

                this->_main_coroutine.Register(this); // re-register
            }
            return *this;
        }

    private:
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase( 
                fiber::StackAllocatorExtern* frame_allocator, 
                F&& function, Args&&... args)
            : _frame_allocator(frame_allocator)
        {
            // make sure that at the constrution of the coroutine the frame_allocator of the task is set
            auto temp = std::exchange(fiber::detail::frame_allocator, _frame_allocator);

            // construct the coroutine
            this->_main_coroutine = std::forward<F>(function)(std::forward<Args>(args)...);

            // reset allocator
            fiber::detail::frame_allocator = temp;

            // registering
            this->_leaf_coroutine = this->_main_coroutine.node();
            this->_main_coroutine.Register(this);
        }
    public:

        /**
         * \brief constructor to create a priority-based task with the lowest priority
         * 
         * Defaults to priority 1 (2nd lowest priority level, lowest priority is 0)
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase(std::string_view task_name, fiber::StackAllocatorExtern* frame_allocator, F&& function, Args&&... args)
            : TaskBase(frame_allocator, std::forward<F>(function), std::forward<Args>(args)...)
        {
            this->_task_name = task_name;
            this->_priority = 1;
            this->_immediatelly_ready = true;
        }

        /**
         * \brief constructor to create a priority-based task that starts immediatelly
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase(std::string_view task_name, uint16_t priority, fiber::StackAllocatorExtern* frame_allocator, F&& function, Args&&... args)
            : TaskBase(frame_allocator, std::forward<F>(function), std::forward<Args>(args)...)
        {
            this->_task_name = task_name;
            this->_priority = priority;
            this->_immediatelly_ready = true;
        }
        

        /**
         * \brief constructor to create a priority-based task that starts at a certain time point in the future
         * 
         * Defaults to priority 1 (2nd lowest priority level, lowest priority is 0)
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase(std::string_view task_name, TimePoint ready, fiber::StackAllocatorExtern* frame_allocator, F&& function, Args&&... args)
            : TaskBase(frame_allocator, std::forward<F>(function), std::forward<Args>(args)...)
        {
            this->_task_name = task_name;
            this->_priority = 1;
            this->_schedule.ready = ready;
            this->_schedule.deadline = ready;
            this->_immediatelly_ready = false;
        }
        
        /**
         * \brief constructor to create a real-time deadline-based task
         * 
         * deadline based tasks automatically have int_max assigned to their priority (highest priority).
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase(std::string_view task_name, TimePoint ready, TimePoint deadline, fiber::StackAllocatorExtern* frame_allocator, F&& function, Args&&... args)
            : TaskBase(frame_allocator, std::forward<F>(function), std::forward<Args>(args)...)
        {
            this->_task_name = task_name;
            this->_priority = _deadline_priority;
            this->_schedule.ready = ready;
            this->_schedule.deadline = deadline;
            this->_immediatelly_ready = false;
        }

        /**
         * \brief constructor to create a real-time deadline-based task
         * 
         * deadline based tasks automatically have int_max assigned to their priority (highest priority).
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr TaskBase(std::string_view task_name, TimePoint ready, Duration deadline, fiber::StackAllocatorExtern* frame_allocator, F&& function, Args&&... args)
            : TaskBase(frame_allocator, std::forward<F>(function), std::forward<Args>(args)...)
        {
            this->_task_name = task_name;
            this->_priority = _deadline_priority;
            this->_schedule.ready = ready;
            this->_schedule.deadline = ready + deadline;
            this->_immediatelly_ready = false;
        }

        virtual ~TaskBase(){}

        /**
         * @brief Overrideable: Gets called if the deadline has been missed and decides wether the task should still execute or not
         * @param d The duration/time that passed since the deadline
         * @details The default version will always return `true` to continue the task
         * @returns Returns if the task should still be executed (`true`) or not (`false`)
         */
        virtual bool missed_deadline([[maybe_unused]]fiber::Duration d){return true;}

        /**
         * @brief Overrideable: Gets called after a `co_await NextCycle;` to calculate the schedule of the next cycle.
         * 
         * Note that `previous_execution.end` is equivalent to the current time .aka `now()`
         * 
         * @param previous_schedule the previous schedule of this task
         * @param previous_execution the previous execution time from the start of the cycle to the end of the cycle
         */
        virtual Schedule next_schedule(
            [[maybe_unused]]Schedule previous_schedule, 
            [[maybe_unused]]ExecutionTime previous_execution)
        {
            return Schedule{previous_execution.end, previous_execution.end};
        }

        /**
         * \brief returns the assigned id of the task.
         * 
         * Has likely been assigned by the scheduler
         */
        constexpr unsigned int id() const {return this->_id;}

        constexpr bool is_deadline_based() const {return this->_priority == _deadline_priority;}
        constexpr bool is_priority_based() const {return this->_priority != _deadline_priority;}

        /**
         * \brief returns the name of the task
         */
        constexpr std::string_view name() const {return this->_task_name;}

        /**
         * \brief returns the maximal allocatable size of the frame (stack allocator)
         */
        constexpr std::size_t max_frame_size() const {return this->_frame_allocator->max_size();}

        /**
         * \brief returns the allocated size of the frame (stack allocator)
         */
        constexpr std::size_t allocated_frame_size() const {return this->_frame_allocator->allocated_size();}

        /**
         * \brief returns the maximal allocated size in the frame since construction (stack allocator)
         */
        constexpr std::size_t max_allocated_frame_size() const {return this->_frame_allocator->max_allocated_size();}

        /**
         * @brief sets a coroutine control signal.
         * 
         * Used by the coroutine or awaitable to set a control signal.
         * Can be used to influence the runtime behaviour of the task, or the way the scheduler continues scheduling it.
         */
        constexpr void signal(const CoSignal& signal){this->_signal = signal;}
        
        /**
         * @brief reads and clears the signal
         */
        constexpr const CoSignal get_signal() {
            CoSignal result = this->_signal;
            this->_signal.none(); // clear signal
            return result;
        }

        /**
         * \brief destroys all contained coroutines
         */
        constexpr void destroy(){this->_main_coroutine.destroy();}
        

        /// @brief Registers the leaf nested coroutine that serves as the resume point after suspensions
        /// @details Meant to be called by the Coroutine
        /// @param leaf a coroutine handle to the leaf tasks
        constexpr void register_leaf(CoroutineNode* leaf) noexcept {
            if(this->_leaf_coroutine){
                this->_leaf_coroutine = leaf;
                this->_instant_resume = true;
            }
        }

        /// @brief Registers a leaf awaitable that will be waited on before resuming its parents coroutine
        /// @details Meant to be called by the Awaitable
        /// @param awaitable The awaitable that will be waited on
        template<class T>
        constexpr void register_leaf(const T* obj, bool (*func)(const T* obj)) noexcept {
            _leaf_awaitable_ready_func = reinterpret_cast<bool (*)(const void*)>(func);
            _leaf_awaitable_obj = reinterpret_cast<const void*>(obj);
        }

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
        constexpr bool is_awaiting() const {
            if(this->_leaf_awaitable_obj!=nullptr){
                return !this->_leaf_awaitable_ready_func(this->_leaf_awaitable_obj);
            }else{
                return false;
            }
        }

        /// @brief returns `true` if the main/root coroutine is done - thus the task is done
        constexpr bool is_done() const {return this->_main_coroutine.is_done();}

        /// @brief same as `is_done()` but for interoperability with `co_await`
        constexpr bool await_ready() const noexcept {return this->is_done();}

        /// @brief interoperability with `co_await` 
        constexpr Exit await_resume();

        constexpr Exit exit_status();

        /// @brief exception handler - will be called if an un-catched exception in a coroutine arises
        #ifndef FIBER_DISABLE_EXCEPTIONS
            // variation using exceptions
            void handle_exception(std::exception_ptr except_ptr);
        #else
            // variation limiting the use of exceptions
            void handle_exception();
        #endif

        TimePoint ready_time() const {return this->_schedule.ready;}
        TimePoint deadline() const {return this->_schedule.deadline;}
        uint32_t priority() const {return this->_priority;}
        bool immediatelly_ready() const {return this->_immediatelly_ready;}

        struct less_priority_s{
            constexpr bool operator () (const TaskBase* lhs, const TaskBase* rhs){
                enum class Cases : unsigned int{
                    both_priority = 0b00,
                    lhs_priority_rhs_deadline = 0b01,
                    lhs_deadline_rhs_priority = 0b10,
                    both_deadline = 0b11
                };
                const Cases Case = static_cast<Cases>((static_cast<unsigned int>(lhs->is_deadline_based()) << 1) || (static_cast<unsigned int>(rhs->is_deadline_based()) << 0));
                bool result = false;
                switch(Case){
                    case Cases::both_priority: result = lhs->_priority < rhs->_priority; break;
                    case Cases::lhs_priority_rhs_deadline: result = true; break;
                    case Cases::lhs_deadline_rhs_priority: result = false; break;
                    case Cases::both_deadline: result = lhs->_schedule.deadline > rhs->_schedule.deadline; break;
                }
                return result;
            }
        };

        struct larger_ready_time_s{
            constexpr bool operator () (const TaskBase* lhs, const TaskBase* rhs){
                return lhs->_schedule.ready > rhs->_schedule.ready;
            }
        };
    };

    /**
     * \brief Wraps awaitables that are not yet derived from `fiber::AwaitableNode`
     * 
     * If wraps arbitrary awaitables that do not derive from `fiber::AwaitableNode` that 
     * implements the registration infrastructure relied on by `fiber::Coroutine` and `fiber::TaskBase`.
     * 
     * Specialises for awaitable passed as lvalues.
     * 
     * Intended to be used only by `CoroutinePromise::await_transform(awaitable)`
     * 
     * \tparam Awaitable an awaitable that implements `.await_ready()`, `.await_resume()` and `.await_suspend(handle)`.
     * 
     * \see fiber::AwaitableNode
     * \see fiber::Coroutine
     * \see fiber::CoroutinePromise
     * \see fiber::TaskBase
     * \see fiber::wrap_awaitable(Awaitable&& awaitable)
     */
    template<class Awaitable, bool is_lvalue=true>
    class AwaitableWrapper{
    private:
        Awaitable& _awaitable;

    public:
        constexpr AwaitableWrapper(Awaitable& awaitable)
            : _awaitable(awaitable){}

        constexpr bool await_ready() const noexcept {
            return this->_awaitable.await_ready();
        }

        static bool s_await_ready(const AwaitableWrapper* This){
            return This->await_ready();
        }

        constexpr auto await_resume() noexcept {
            return this->_awaitable.await_resume();
        }

        constexpr CoSignal await_suspend_signal() const noexcept {return CoSignal().await();}

        template<class ReturnType>
        constexpr auto await_suspend(std::coroutine_handle<fiber::CoroutinePromise<ReturnType>> handle) noexcept {
            // register leaf in task to tell it what awaitable to wait for
            handle.promise().task()->register_leaf(this, AwaitableWrapper::s_await_ready);

            handle.promise().task()->signal(await_suspend_signal());

            // await_suspend is optional, so check if the Type supports it at compile time and call it conditionally
            if constexpr (requires {this->_awaitable.await_suspend(handle);} ){
                return this->_awaitable.await_suspend(handle);
            }else{
                return void();
            }
        }
    };

    /**
     * \brief Wraps awaitables that are not yet derived from `fiber::AwaitableNode`
     * 
     * If wraps arbitrary awaitables that do not derive from `fiber::AwaitableNode` that 
     * implements the registration infrastructure relied on by `fiber::Coroutine` and `fiber::TaskBase`.
     * 
     * Specialises for awaitable passed as rvalues.
     * 
     * Intended to be used only by `CoroutinePromise::await_transform(awaitable)`
     * 
     * \tparam Awaitable an awaitable that implements `.await_ready()`, `.await_resume()` and `.await_suspend(handle)`.
     * 
     * \see fiber::AwaitableNode
     * \see fiber::Coroutine
     * \see fiber::CoroutinePromise
     * \see fiber::TaskBase
     * \see fiber::wrap_awaitable(Awaitable&& awaitable)
     */
    template<class Awaitable>
    class AwaitableWrapper<Awaitable, false>{
    private:
        Awaitable _awaitable;

    public:
        constexpr AwaitableWrapper(Awaitable&& awaitable)
            : _awaitable(std::move(awaitable)){}

        constexpr bool await_ready() const noexcept {
            return this->_awaitable.await_ready();
        }

        static bool s_await_ready(const AwaitableWrapper* This){
            return This->await_ready();
        }

        constexpr auto await_resume() noexcept {
            return this->_awaitable.await_resume();
        }

        constexpr CoSignal await_suspend_signal() const noexcept {return CoSignal().await();}

        template<class ReturnType>
        constexpr auto await_suspend(std::coroutine_handle<fiber::CoroutinePromise<ReturnType>> handle) noexcept {
            // register leaf in task to tell it what awaitable to wait for
            handle.promise().task()->register_leaf(this, AwaitableWrapper::s_await_ready);

            handle.promise().task()->signal(await_suspend_signal());

            // await_suspend is optional, so check if the Type supports it at compile time and call it conditionally
            if constexpr (requires {this->_awaitable.await_suspend(handle);} ){
                return this->_awaitable.await_suspend(handle);
            }else{
                return void();
            }
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
     * \see fiber::AwaitableWrapper
     * \see fiber::AwaitableNode
     */
    template<class Awaitable>
    AwaitableWrapper<Awaitable, std::is_lvalue_reference<Awaitable>::value> wrap_awaitable(Awaitable&& awaitable){
        if constexpr (std::is_lvalue_reference<Awaitable>::value){
            return AwaitableWrapper<Awaitable, true>(awaitable);
        }else{
            return AwaitableWrapper<Awaitable, false>(std::move(awaitable));
        }
    }

    /**
     * \brief Promise type for an fiber::Coroutine
     * 
     * This class implements the coroutine promise required by the C++20 coroutine machinery.
     * `CoroutinePromise<ReturnType>` is instantiated by the compiler when a coroutine with `Coroutine<ReturnType>` as its return type is used.
     * 
     * ### Awaitable Support
     * The `await_transform()` method is used to wrap arbitrary awaitables unless they already derive from a framework-specific `AwaitableNode`.
     * This supports customization of how `co_await` expressions behave within the coroutine body.
     * 
     * ### Memory Management
     * Overrides `operator new` and `operator delete` to allocate memory for the coroutine frame from a custom allocator.
     * In the usual cast this allocator is provided by the root task of the coroutine chain
     * 
     * \see fiber::Coroutine
     * \see fiber::CoroutineNode
     * \see fiber::AwaitableWrapper
     */
    template<class ReturnType=void>
    class CoroutinePromise : public CoroutineNode{
    private:
        ReturnType _return_value;
    
    public:

        inline CoroutinePromise(){
            this->CoroutineNode::_handle = std::coroutine_handle<CoroutinePromise>::from_promise(*this);
        }

        template<class Awaitable>
        constexpr auto await_transform(Awaitable&& awaitable){
            if constexpr (std::same_as<Awaitable, Delay> || std::same_as<Awaitable, NextCycle> || is_coroutine_v<Awaitable>){
                return std::forward<Awaitable>(awaitable);
            }else{
                return wrap_awaitable(std::forward<Awaitable>(awaitable));
            }
        }

        inline auto get_return_object(){return Coroutine<ReturnType>(std::coroutine_handle<CoroutinePromise>::from_promise(*this));}
        

        /// @brief Print an error message and kill the task
        constexpr void unhandled_exception() noexcept {
            #ifndef FIBER_DISABLE_EXCEPTIONS
                this->task()->handle_exception(std::current_exception());
            #else
                this->task()->handle_exception();
            #endif
        } 

        constexpr void return_value(const ReturnType& value){this->_return_value = value;}
        constexpr void return_value(ReturnType&& value){this->_return_value = std::move(value);}

        constexpr ReturnType&& get_return_value() {
            FIBER_ASSERT_INTERNAL_MSG(this->is_done(), "Tried to get coroutine return value, before `is_done()`. S: Read value after the coroutine is finished. Check for `is_done()` or use co_await");
            return std::move(_return_value);
        }

        constexpr static void* operator new(std::size_t size){
            return fiber::detail::frame_allocator->allocate(size);
        }

        constexpr static void operator delete(void* ptr, std::size_t size){
            fiber::detail::frame_allocator->deallocate(ptr, size);
        }
        
        constexpr bool await_ready() const noexcept {return this->is_done();}

        constexpr ReturnType await_resume() {
            FIBER_ASSERT_INTERNAL_MSG(this->await_ready(), "Resume on unready coroutnie awaitable. S: Check `await_ready()` before calling `resume()`.");
            return std::move(this->_return_value); // move out before destroying to prevent use after free
        }
    };

    /**
     * \brief Specialisation for ReturnTypes of type `void`
     */
    template<>
    class CoroutinePromise<void> : public CoroutineNode{
    public:

        inline CoroutinePromise(){
            this->CoroutineNode::_handle = std::coroutine_handle<CoroutinePromise>::from_promise(*this);
        }

        template<class Awaitable>
        constexpr auto await_transform(Awaitable&& awaitable){
            if constexpr (std::same_as<Awaitable, Delay> || std::same_as<Awaitable, NextCycle> || std::derived_from<Awaitable, CoroutineNode>){
                return std::forward<Awaitable>(awaitable);
            }else{
                return wrap_awaitable(std::forward<Awaitable>(awaitable));
            }
        }

        inline auto get_return_object(){return Coroutine<void>(std::coroutine_handle<CoroutinePromise>::from_promise(*this));}
        

        /// @brief Print an error message and kill the task
        inline void unhandled_exception() noexcept {
            #ifndef FIBER_DISABLE_EXCEPTIONS
                this->task()->handle_exception(std::current_exception());
            #else
                this->task()->handle_exception();
            #endif
        } 

        inline static void* operator new(std::size_t size){
            return fiber::detail::frame_allocator->allocate(size);
        }

        inline static void operator delete(void* ptr, std::size_t size){
            fiber::detail::frame_allocator->deallocate(ptr, size);
        }
        
        inline bool await_ready() const noexcept {return this->is_done();}

        inline void await_resume() {
            FIBER_ASSERT_INTERNAL_MSG(this->await_ready(), "Resume on unready coroutnie awaitable. S: Check `await_ready()` before calling `resume()`.");
        }
    };

//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   Coroutine: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    template<class T>
    constexpr Coroutine<T>::~Coroutine() noexcept {
        this->destroy();
    }

    template<class T>
    constexpr void Coroutine<T>::resume() {this->coro.resume();}

    template<class T>
    constexpr void Coroutine<T>::destroy() {
        if(this->coro != nullptr){
            this->coro.promise().destroy();
            this->coro = nullptr;
        } 
    }

    template<class T>
    constexpr void Coroutine<T>::Register(TaskBase* task){this->coro.promise().Register(task);}

    template<class T>
    constexpr bool Coroutine<T>::is_done() const {return this->coro.done();}

    template<class T>
    constexpr Coroutine<T>::operator bool() const noexcept {return this->coro != nullptr;}

    template<class T>
    constexpr CoroutinePromise<T>& Coroutine<T>::promise() {return this->coro.promise();}

    template<class T>
    constexpr CoroutineNode* Coroutine<T>::node() {return &this->coro.promise();}

    template<class T>
    constexpr const CoroutineNode* Coroutine<T>::node() const {return &this->coro.promise();}

    template<class T>
    constexpr bool Coroutine<T>::await_ready() const noexcept {
        return this->coro.promise().await_ready();
    }
    
    template<class ReturnValue>
    template<class T>
    constexpr void Coroutine<ReturnValue>::await_suspend(std::coroutine_handle<CoroutinePromise<T>> handle) noexcept {
        this->coro.promise().await_suspend(handle);
    }

    template<class T>
    constexpr T Coroutine<T>::await_resume() {
        FIBER_ASSERT_INTERNAL_MSG(this->coro, "Attempting to read from a deleted coroutine. S: Remove double `await_resume()`. Check for call to `await_resume()` after `co_await`.");
        return this->coro.promise().await_resume();
    }

    template<>
    constexpr void Coroutine<void>::await_resume() {
        FIBER_ASSERT_INTERNAL_MSG(this->coro, "Attempting to read from a deleted coroutine. S: Remove double `await_resume()`. Check for call to `await_resume()` after `co_await`.");
    }
    
//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   TaskBase: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    constexpr Exit TaskBase::await_resume(){
        FIBER_ASSERT_INTERNAL(this->is_done());
        return this->_main_coroutine.promise().get_return_value();
    }
    
    constexpr Exit TaskBase::exit_status() {
        FIBER_ASSERT_INTERNAL(this->is_done());
        return this->_main_coroutine.promise().get_return_value();
    }

//------------------------------------------------------------------------------------------------------------------------------------------
//                                                   CoroutineNode: Implementation
//------------------------------------------------------------------------------------------------------------------------------------------

    inline void CoroutineNode::destroy() noexcept {
        auto temp = std::exchange(fiber::detail::frame_allocator, this->task()->_frame_allocator);
        this->_handle.destroy();
        fiber::detail::frame_allocator = temp;  
    };

    template<class ReturnType>
    constexpr void CoroutineNode::await_suspend(std::coroutine_handle<CoroutinePromise<ReturnType>> handle) noexcept {
        // store parent and task
        this->_parent = &handle.promise();
        this->_task = handle.promise().task();

        // register leafe in task to tell it what coroutine to resume next
        this->_task->register_leaf(this);
    }

    constexpr std::suspend_always CoroutineNode::final_suspend() noexcept {
        // unregister self and re-register the parent, so next time the task will resume the parent
        if(this->_parent != nullptr){
            this->_task->register_leaf(this->_parent);
        }
        return std::suspend_always{};
    }

}