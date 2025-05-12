#pragma once

// std
#include <variant>
#include <concepts>
#include <ranges>
#include <string_view>

// embed
#include <embed/Chrono/TimePoint.hpp>
#include <embed/Containers/ArrayList.hpp>
#include <embed/Containers/DualPriorityQueue.hpp>
#include <embed/OS/RealTimeTask.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/OStream/ansi.hpp>
#include <embed/OStream/utf8_lines.hpp>

namespace embed
{
    
    /**
     * @brief Default implementation for a function that should send the MCU to sleep until `time`, but does nothing.
     */
    void default_sleep_until([[maybe_unused]]TimePoint time){
        return;
    }

    /**
     * @brief The concept for a logging device that can be passed into the RealTimeScheduler
     */
    template<class Logger>
    concept CRealTimeSchedulerLogger = requires(
        TimePoint time, 
        TimePoint time_after, 
        TimePoint time_until, 
        std::string_view name, 
        unsigned int id, 
        std::string_view from_queue, 
        std::string_view to_queue) 
    {
        { Logger::log_add(time, name, id, to_queue) };
        { Logger::log_move(time, name, id, from_queue, to_queue) };
        { Logger::log_resume(time, time_after, name, id) };
        { Logger::log_delete(time, name,id) };
        { Logger::log_sleep(time, time_until) };
    };

    /**
     * @brief A default logger that does nothing
     */
    class NullLogger {
    public:

        static inline void log_add([[maybe_unused]]TimePoint time, [[maybe_unused]]std::string_view name, [[maybe_unused]]unsigned int id, [[maybe_unused]]std::string_view to_queue){}
        static inline void log_move([[maybe_unused]]TimePoint time, [[maybe_unused]]std::string_view name, [[maybe_unused]]unsigned int id, [[maybe_unused]]std::string_view from_queue, [[maybe_unused]]std::string_view to_queue){}
        static inline void log_resume([[maybe_unused]]TimePoint time_from, [[maybe_unused]]TimePoint time_after, [[maybe_unused]]std::string_view name, [[maybe_unused]]unsigned int id){}
        static inline void log_delete([[maybe_unused]]TimePoint time, [[maybe_unused]]std::string_view name, [[maybe_unused]]unsigned int id){}
        static inline void log_sleep([[maybe_unused]]TimePoint time, [[maybe_unused]]TimePoint sleep_until){}

    };

    /**
     * @brief A logger that will write all loggings directly to an output stream.
     * 
     * To set its output stream write to:
     * ```
     * OutputLogger<MyClock>::stream = myStream;
     * ```
     * 
     * @tparam Clock A Clock type that conforms to the `embed::CClock` concept.
     * 
     * @see embed::CClock
     * @see embed::RealTimeScheduler
     */
    class OutputLogger {
    public:

        static inline OStreamRef stream;

        static void print_symbol(std::string_view symbol){
            stream << embed::ansi::blue << symbol;
        }

        static void print_time_point(TimePoint time){
            stream << embed::ansi::blue << time;
        }

        static void print_time_point(Duration Duration){
            stream << embed::ansi::light_blue << Duration;
        }

        static void print_primary_action(std::string_view action){
            stream << " " << embed::ansi::light_orange << action << " ";
        }

        static void print_secondary_action(std::string_view action){
            stream << " " << embed::ansi::blue << action << " ";
        }

        static void print_name(std::string_view name){
            stream << embed::ansi::light_blue << name;
        }

        static void print_id(unsigned int id){
            stream << embed::ansi::light_yellow << id;
        }

        static void print_time(TimePoint time){
            using namespace std::string_view_literals;
            print_symbol("@"sv);
            print_time_point(time);
        }

        static void print_task(std::string_view name, unsigned int id){
            using namespace std::string_view_literals;
            print_symbol("{"sv);
            print_name(name);
            print_symbol(", "sv);
            print_id(id);
            print_symbol("}"sv);
        }

        static void log_add(TimePoint time, std::string_view name, unsigned int id, std::string_view to_queue){
            using namespace std::string_view_literals;
            print_time(time);
            print_primary_action("add"sv);
            print_task(name, id);
            print_secondary_action("to"sv);
            print_name(to_queue);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_move(TimePoint time, std::string_view name, unsigned int id, std::string_view from_queue, std::string_view to_queue){
            using namespace std::string_view_literals;
            print_time(time);
            print_primary_action("move"sv);
            print_task(name, id);
            print_secondary_action("from"sv);
            print_name(from_queue);
            print_secondary_action("to"sv);
            print_name(to_queue);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_resume(TimePoint time_from, TimePoint time_after, std::string_view name, unsigned int id){
            using namespace std::string_view_literals;
            print_time(time_from);
            print_primary_action("resume"sv);
            print_task(name, id);
            print_secondary_action("time"sv);
            print_time_point(time_after - time_from);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_delete(TimePoint time, std::string_view name, unsigned int id){
            using namespace std::string_view_literals;
            print_time(time);
            print_primary_action("delete"sv);
            print_task(name, id);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_sleep(TimePoint time, TimePoint sleep_until){
            using namespace std::string_view_literals;
            print_time(time);
            print_primary_action("sleep until"sv);
            print_time_point(sleep_until);
            stream << embed::ansi::reset << embed::endl;
        }

    };

    /**
     * @brief A real time scheduler that starts tasks once they are ready and schedules them by earliest deadline first
     * 
     * Manages three lists of tasks:
     * - waiting: a priority list, sorted by the earliest ready times.
     * - running: a priority list, sorted by the earliest deadlines.
     * - awaiting: a list containing all tasks that are waiting on an awaitable or future.
     * 
     * @tparam n_tasks The maximum number of thats that will be pre-allocated for this scheduler.
     * @tparam logger A logger that implements the functions defined by `embed::CRealTimeSchedulerLogger`
     */
    template<size_t n_tasks, CRealTimeSchedulerLogger logger = NullLogger>
    class RealTimeScheduler {
    private:
        using dual_priority_queue_type = DualPriorityQueue<RealTimeTask*, n_tasks, larger_ready_time, larger_deadline>;
        using dual_array_list_type = DualArrayList<RealTimeTask*, n_tasks>;

        using waiting_queue_ref = Stage1DualPriorityQueueRef<RealTimeTask*, n_tasks, larger_ready_time, larger_deadline>;
        using running_queue_ref = Stage2DualPriorityQueueRef<RealTimeTask*, n_tasks, larger_ready_time, larger_deadline>;

        using waiting_queue_const_ref = Stage1DualPriorityQueueConstRef<RealTimeTask*, n_tasks, larger_ready_time, larger_deadline>;
        using running_queue_const_ref = Stage2DualPriorityQueueConstRef<RealTimeTask*, n_tasks, larger_ready_time, larger_deadline>;
        
        TimePoint (*_now)(); // function pointer to a function returning the current time
        void (*_sleep_until)(TimePoint); // function pointer to a function returning the current time

        // TODO: dual priority queue with an unordered list to save more memory
        //       [stage 2 priority queue][reserve][unordered list][reserve][stage 1 priority list]
        //       consider if the complexity is worth it - probably not!
        dual_priority_queue_type _priority_queue; // ready + deadline
        ArrayList<RealTimeTask*, n_tasks> _await_bench;
        unsigned int _next_task_id = 0; // next id for the next added task


    private:

        waiting_queue_ref waiting_queue(){return this->_priority_queue;}
        running_queue_ref running_queue(){return this->_priority_queue;}

        waiting_queue_const_ref waiting_queue() const {return this->_priority_queue;}
        running_queue_const_ref running_queue() const {return this->_priority_queue;}

        /**
         * @brief Moves tasks that got ready from the waiting- and awaiting-queue into the running queue
         * 
         * 1. Checks all tasks from the awaiting-queue that return `true` on `.await_ready()` and moves them into the running queue.
         * 2. Moves the top of the waiting priority queue that got ready into the running queue.
         */
        void promote(){
            // promote await back into running queue
            for(RealTimeTask* task : this->_await_bench){
                if(!task->is_awaiting()){
                    logger::log_move(this->now(), task->name(), task->id(), "await", "run");
                    this->running_queue().push(task);
                }
            }
            this->_await_bench.erase_if([](const RealTimeTask* task){return !task->is_awaiting();});

            // promote waiting queue into running queue
            while(!this->waiting_queue().empty()){
                RealTimeTask* task = this->waiting_queue().top();
                const TimePoint now = this->now();
                if(task->ready_time() <= now){
                    logger::log_move(now, task->name(), task->id(), "wait", "run");
                    this->waiting_queue().pop();
                    this->running_queue().push(task);
                }else{
                    return;
                }
            }
        }

        /**
         * \brief Sleep until the top of the waiting list
         */
        void sleep(){
            this->_sleep_until(this->waiting_queue().top()->ready_time());
        }

        /**
         * @brief Runs the next task and decides to which queue it belongs after running.
         * 
         * Runs the task with the highest priority (= lowest deadline) from the running queue. 
         * The task may send a signal using `embed::CoSignal` from an `embed::AwaitableNode`.
         * If the task sent a: 
         * - <b><code>NextCycle</code> signal</b>: the scheduler will call the tasks `.next_schedule()` method to calculate its next schedule and puts it back into the waiting priority list.
         * - <b><code>Await</code> signal</b>: the task will be put on the `await queue` until the awaitable signals `true` on `.await_ready()`.
         * - <b><code>Implicit/ExplicitDelay</code> signal</b>: the scheduler will calculate the next schedule of the task using the given delay.
         * - <b><code>None</code></b>: (happens when the task ends) the task will be removed from the scheduler and not put back into any queue.
         * 
         * @see embed::CoSignal
         * @see embed::AwaitableNode
         */
        void run_next(){
            RealTimeTask* task = this->running_queue().top_pop();
            task->_execution_start = this->now();
            task->resume();
            logger::log_resume(task->_execution_start, this->now(), task->name(), task->id());
            // re-schedule
            
            const CoSignal signal = task->get_signal();
            switch(signal.type()){
                case CoSignal::Type::Await : {
                    // clear signal
                    this->_await_bench.emplace_back(task);
                    logger::log_move(this->now(), task->name(), task->id(), "resume", "await");
                }break;
                case CoSignal::Type::NextCycle : {
                    task->_schedule = task->next_schedule(task->_schedule, ExecutionTime{task->_execution_start, this->now()});
                    this->waiting_queue().push(task);
                    logger::log_move(this->now(), task->name(), task->id(), "resume", "wait");
                }break;
                case CoSignal::Type::ImplicitDelay : {
                    const Duration rel_deadline = task->_schedule.deadline - task->_schedule.ready;
                    task->_schedule.ready = this->now() + embed::rounding_duration_cast<Duration>(signal.delay());
                    task->_schedule.deadline = task->_schedule.ready + rel_deadline;
                    this->waiting_queue().push(task);
                    logger::log_move(this->now(), task->name(), task->id(), "resume", "wait");
                }break;
                case CoSignal::Type::ExplicitDelay : {
                    task->_schedule.ready = this->now() + embed::rounding_duration_cast<Duration>(signal.delay());
                    task->_schedule.deadline = task->_schedule.ready + embed::rounding_duration_cast<Duration>(signal.deadline());
                    logger::log_move(this->now(), task->name(), task->id(), "resume", "wait");
                }break;
                case CoSignal::Type::None : {
                    /* 
                    - Nothing to do 
                    - task probably finished 
                    - let it die by not inserting it into lists
                    */
                    logger::log_delete(this->now(), task->name(), task->id());
                } break;
                default : {
                    /* 
                    - Nothing to do 
                    - task probably finished 
                    - let it die by not inserting it into lists
                    */
                    logger::log_delete(this->now(), task->name(), task->id());
                } break;
            }
        }

    public:

        RealTimeScheduler(TimePoint (*now)(), void (*sleep_until)(TimePoint) = default_sleep_until) 
            : _now(now)
            , _sleep_until(sleep_until){}

        /**
         * @brief returns the current time
         * 
         * Uses the function passed at construction
         * 
         * @returns a TimePoint containing the current time
         */
        TimePoint now() const {return this->_now();}

        /**
         * @brief Adds a tasks to the scheduler
         * 
         * Assigns an unique-ID to the task and adds it either to the 'running' or 'waiting' queue.
         * 
         * @throws Throws an `AssertionFailureO1` if `EMBED_ASSERTION_LEVEL_O1` or higher is enabled, if the task could not be added and the scheduler is already full.
         */
        void add(RealTimeTask* task){
            task->id(this->_next_task_id++);
            EMBED_ASSERT_O1_MSG(!this->is_full(), "Scheduler is full and cannot handle more tasks safely. S: Increase the storage capacity for the number of tasks in the template parameter `n_taks`.");
            const TimePoint now = this->now();
            if(task->ready_time() <= now){
                logger::log_add(now, task->name(), task->id(), "run");
                this->running_queue().push(task);
            }else{
                logger::log_add(now, task->name(), task->id(), "wait");
                this->waiting_queue().push(task);
            }
        }

        /**
         * @brief Checks the state of Tasks and executes one if ready
         * 
         * First promotes tasks from the \em waiting-queue or the \em awaiting-queue to the \em running-queue if they are ready.
         * Then runs the next task with the earliest deadline
         */
        void spin(){
            this->promote();
            if(this->is_busy()){
                this->run_next();
            }else{
                this->sleep();
            }
        }

        /**
         * @brief returns the capacity of the scheduler
         * 
         * The capacity represents the number of total tasks that can be added without reallocation.
         * For static schedulers (like this one) `.capacity()` is equivalent to `.max_size()`.
         * 
         * To increase the capacity increase the template parameter `n_tasks`.
         */ 
        constexpr size_t capacity() const {return n_tasks;}

        /**
         * @brief returns the maximal number of tasks that this scheduler can manage.
         * 
         * To increase the `max_size()` increase the template parameter `n_tasks`.
         */
        constexpr size_t max_size() const {return n_tasks;}

        /**
         * @brief returns the number of tasks currently in the waiting queue
         * 
         * Tasks that are in the waiting queue are waiting for time to pass until `this->now()`
         * is larger than the ready time of a task.
         */
        constexpr size_t n_waiting() const {return this->waiting_queue().size();}

        /**
         * @brief returns the number of tasks currently in the running queue
         * 
         * Tasks that are in the ready queue are all tasks that have a ready time that is larger
         * than `this->now()`.
         */
        constexpr size_t n_running() const {return this->running_queue().size();}

        /**
         * @brief returns the number of tasks currently in the awaiting queue
         * 
         * Tasks that are in the awaiting queue are waiting for a future or awaitable to become ready.
         */
        constexpr size_t n_awaiting() const {return this->_await_bench.size();}

        /**
         * @brief returns the current number of tasks that this scheduler manages.
         */
        constexpr size_t size() const {return this->n_waiting() + this->n_running() + this->n_awaiting();}

        /**
         * @brief returns the remaining number of tasks that can still added to the scheduler
         */
        constexpr size_t reserve() const {return this->capacity() - this->size();}

        

        /**
         * @brief returns `true` if there are no tasks in its running queue
         */
        constexpr bool is_waiting() const {
            return this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are tasks in its running queue and `false` if the scheduler is waiting.
         */
        constexpr bool is_busy() const {
            return !this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are no tasks in any queue
         */
        constexpr bool is_empty() const {
            return this->_priority_queue.empty() && this->_await_bench.empty();
        }

        /**
         * @brief returns `true` if the scheduler is full, the scheduler cannot handle more tasks, no more tasks can be added to the scheduler safely.
         */
        constexpr bool is_full() const {return this->reserve() == 0;}

        /**
         * @brief returns `true` if there are no tasks in the sheduler
         */
        constexpr bool is_done() const {
            return this->_priority_queue.empty() && this->_await_bench.empty();
        }

    private:

        /**
         * @brief prints a scheduler queue to the stream
         * @param stream An `embed::OStream` reference
         * @param taskList A range that contains `RealTimeTask*` objects
         */
        template<std::ranges::range TaskList>
        static void print_task_list(OStream& stream, const TaskList& taskList, const int indentation = 0){
            using namespace std::string_view_literals;
            using namespace embed::utf8_lines;

            // get length of longest name
            int max_name_length = 4;
            for(const RealTimeTask* task : taskList){
                const int name_size = static_cast<int>(task->name().size());
                max_name_length =  (name_size > max_name_length) ? name_size : max_name_length;
            }

            const int id_width = 12;
            const int time_width = 24;

            // table top
            stream.put(' ', indentation);
            stream << single_corner_topleft << single_horizontal;
            for(int i = 0; i < max_name_length; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_up << single_horizontal;
            for(int i = 0; i < id_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_up << single_horizontal;
            for(int i = 0; i < time_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_up << single_horizontal;
            for(int i = 0; i < time_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_corner_topright;
            stream << embed::newl;

            // print header
            stream.put(' ', indentation);
            stream << single_vertical << ' ';
            stream << FormatStr("name"sv).mwidth(max_name_length).left();
            stream << ' ' << single_vertical << ' ';
            stream << FormatStr("id"sv).mwidth(id_width).right();
            stream << ' ' << single_vertical << ' ';
            stream << FormatStr("ready"sv).mwidth(time_width).right();
            stream << ' ' << single_vertical << ' ';
            stream << FormatStr("deadline"sv).mwidth(time_width).right();
            stream << ' ' << single_vertical << embed::newl;

            // header underline
            stream.put(' ', indentation);
            stream << mixed_t_left << double_horizontal;
            for(int i = 0; i < max_name_length; ++i) stream << double_horizontal;
            stream << double_horizontal << mixed_cross << double_horizontal;
            for(int i = 0; i < id_width; ++i) stream << double_horizontal;
            stream << double_horizontal << mixed_cross << double_horizontal;
            for(int i = 0; i < time_width; ++i) stream << double_horizontal;
            stream << double_horizontal << mixed_cross << double_horizontal;
            for(int i = 0; i < time_width; ++i) stream << double_horizontal;
            stream << double_horizontal << mixed_t_right;
            stream << embed::newl;

            // print table
            for(const RealTimeTask* task : taskList){
                stream.put(' ', indentation);
                stream << single_vertical << ' ';
                stream << FormatStr(task->name()).mwidth(max_name_length).left();
                stream << ' ' << single_vertical << ' ';
                stream << FormatInt(task->id()).mwidth(id_width).right();
                stream << ' ' << single_vertical << ' ';
                stream << format_chrono(task->ready_time().time_since_epoch()).mwidth(time_width).right();
                stream << ' ' << single_vertical << ' ';
                stream << format_chrono(task->deadline().time_since_epoch()).mwidth(time_width).right();
                stream << ' ' << single_vertical << embed::newl;
            }

            // table bottom
            stream.put(' ', indentation);
            stream << single_corner_botleft << single_horizontal;
            for(int i = 0; i < max_name_length; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_down << single_horizontal;
            for(int i = 0; i < id_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_down << single_horizontal;
            for(int i = 0; i < time_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_t_down << single_horizontal;
            for(int i = 0; i < time_width; ++i) stream << single_horizontal;
            stream << single_horizontal << single_corner_botright;
            stream << embed::newl;
        }
    public:

        /**
         * @brief prints the current state of the scheduler. Lists all queues and contained tasks.
         * 
         * Example output:
         * ```
         * @2us Running:
         *   ┌────────┬──────────────┬──────────────────────────┬──────────────────────────┐
         *   │ name   │           id │                    ready │                 deadline │
         *   ╞════════╪══════════════╪══════════════════════════╪══════════════════════════╡
         *   │ Task 1 │            0 │                      1us │                      5us │
         *   └────────┴──────────────┴──────────────────────────┴──────────────────────────┘
         *
         * @2us Waiting:
         *   ┌──────────┬──────────────┬──────────────────────────┬──────────────────────────┐
         *   │ name     │           id │                    ready │                 deadline │
         *   ╞══════════╪══════════════╪══════════════════════════╪══════════════════════════╡
         *   │ Task two │            1 │                      2us │                      4us │
         *   └──────────┴──────────────┴──────────────────────────┴──────────────────────────┘
         *
         * @2us Awaiting:
         *   ┌──────┬──────────────┬──────────────────────────┬──────────────────────────┐
         *   │ name │           id │                    ready │                 deadline │
         *   ╞══════╪══════════════╪══════════════════════════╪══════════════════════════╡
         *   └──────┴──────────────┴──────────────────────────┴──────────────────────────┘
         * ```
         * 
         * @param stream A reference to an `embed::OStream` object
         */
        void print(OStream& stream) const {
            const TimePoint now = this->now();
            stream << "@" << now << " Running: " << embed::newl;
            RealTimeScheduler::print_task_list(stream, this->running_queue(), 2);
            stream << embed::newl << "@" << now << " Waiting: " << embed::newl;
            RealTimeScheduler::print_task_list(stream, this->waiting_queue(), 2);
            stream << embed::newl << "@" << now << " Awaiting: " << embed::newl;
            RealTimeScheduler::print_task_list(stream, this->_await_bench, 2);
            stream << embed::newl;
        }
        
        /**
         * @brief prints the state of the scheduler. Lists all queues and their contained tasks.
         */
        friend OStream& operator<<(OStream& stream, const RealTimeScheduler& scheduler){
            scheduler.print(stream);
            return stream;
        }

    };
    
    

} // namespace embed
