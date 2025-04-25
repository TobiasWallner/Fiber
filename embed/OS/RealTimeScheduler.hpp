#pragma once

// std
#include <variant>

// embed
#include <embed/OS/Clock.hpp>
#include <embed/Containers/ArrayList.hpp>
#include <embed/Containers/DualPriorityQueue.hpp>
#include <embed/OS/RealTimeTask.hpp>

namespace embed
{
    
    /**
     * @brief Default implementation for a function that should send the MCU to sleep until `time`, but does nothing.
     */
    template<CClock Clock>
    void default_sleep_until([[maybe_unused]]typename Clock::time_point time){
        return;
    }

    template<class Logger>
    concept CRealTimeSchedulerLogger = requires(
        Logger::time_point time, 
        Logger::time_point time_after, 
        Logger::time_point time_until, 
        const char* name, 
        unsigned int id, 
        const char* from_queue, 
        const char* to_queue) 
    {
        typename Logger::time_point;

        { Logger::log_add(time, name, id, to_queue) };
        { Logger::log_move(time, name, id, from_queue, to_queue) };
        { Logger::log_resume(time, time_after, name, id) };
        { Logger::log_delete(time, name,id) };
        { Logger::log_sleep(time, time_until) };
    };

    template<CClock Clock>
    class NullLogger {
    public:
        using cock = Clock;
        using time_point = typename Clock::time_point;
        using duration = typename Clock::duration;

        static inline void log_add([[maybe_unused]]time_point time, [[maybe_unused]]const char* name, [[maybe_unused]]unsigned int id, [[maybe_unused]]const char* to_queue){}
        static inline void log_move([[maybe_unused]]time_point time, [[maybe_unused]]const char* name, [[maybe_unused]]unsigned int id, [[maybe_unused]]const char* from_queue, [[maybe_unused]]const char* to_queue){}
        static inline void log_resume([[maybe_unused]]time_point time_from, [[maybe_unused]]time_point time_after, [[maybe_unused]]const char* name, [[maybe_unused]]unsigned int id){}
        static inline void log_delete([[maybe_unused]]time_point time, [[maybe_unused]]const char* name, [[maybe_unused]]unsigned int id){}
        static inline void log_sleep([[maybe_unused]]time_point time, [[maybe_unused]]time_point sleep_until){}

    };

    template<CClock Clock>
    class OutputLogger {
    public:
        using cock = Clock;
        using time_point = typename Clock::time_point;
        using duration = typename Clock::duration;

        static inline OStreamRef stream;

        static void print_symbol(const char* symbol){
            stream << embed::ansi::blue << symbol;
        }

        static void print_time_point(time_point time){
            stream << embed::ansi::blue << time;
        }

        static void print_time_point(duration duration){
            stream << embed::ansi::light_blue << duration;
        }

        static void print_primary_action(const char* action){
            stream << " " << embed::ansi::light_orange << action << " ";
        }

        static void print_secondary_action(const char* action){
            stream << " " << embed::ansi::blue << action << " ";
        }

        static void print_name(const char* name){
            stream << embed::ansi::light_blue << name;
        }

        static void print_id(unsigned int id){
            stream << embed::ansi::light_yellow << id;
        }

        static void print_time(time_point time){
            print_symbol("@");
            print_time_point(time);
        }

        static void print_task(const char* name, unsigned int id){
            print_symbol("{");
            print_name(name);
            print_symbol(", ");
            print_id(id);
            print_symbol("}");
        }

        static void log_add(time_point time, const char* name, unsigned int id, const char* to_queue){
            print_time(time);
            print_primary_action("add");
            print_task(name, id);
            print_secondary_action("to");
            print_name(to_queue);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_move(time_point time, const char* name, unsigned int id, const char* from_queue, const char* to_queue){
            print_time(time);
            print_primary_action("move");
            print_task(name, id);
            print_secondary_action("from");
            print_name(from_queue);
            print_secondary_action("to");
            print_name(to_queue);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_resume(time_point time_from, time_point time_after, const char* name, unsigned int id){
            print_time(time_from);
            print_primary_action("resume");
            print_task(name, id);
            print_secondary_action("time");
            print_time_point(time_after - time_from);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_delete(time_point time, const char* name, unsigned int id){
            print_time(time);
            print_primary_action("delete");
            print_task(name, id);
            stream << embed::ansi::reset << embed::endl;
        }

        static void log_sleep(time_point time, time_point sleep_until){
            print_time(time);
            print_primary_action("sleep until");
            print_time_point(sleep_until);
            stream << embed::ansi::reset << embed::endl;
        }

    };

    /**
     * @brief A real time scheduler that starts tasks once they are ready and schedules them by earliest deadline first
     * 
     * 
     * @tparam Clock The `embed::CClock` like clock that the scheduler should use. Defines the tick type, timer overflow, duration, time point and `now()` function.
     * @tparam n_tasks The maximum number of thats that will be pre-allocated for this scheduler.
     * @tparam FSleepUntil A function in the form `void sleep_until(Clock::time_point time)` that will be called if there is nothing to do and the MCU can enter sleep mode.
     * @tparam logger A logger that implements the functions defined by `embed::CRealTimeSchedulerLogger`
     * 
     */
    template<
        CClock Clock, size_t n_tasks, 
        void (*FSleepUntil)(typename Clock::time_point) = default_sleep_until<Clock>, 
        CRealTimeSchedulerLogger logger = NullLogger<Clock>>
    class RealTimeScheduler {
    public:
        using clock = Clock;
        using time_point = typename Clock::time_point;
        using duration = typename Clock::duration;

    private:
        using RTTask = RealTimeTask<Clock>;
        using dual_priority_queue_type = DualPriorityQueue<RTTask*, n_tasks, smaller_ready_time<Clock>, smaller_deadline<Clock>>;
        using dual_array_list_type = DualArrayList<RTTask*, n_tasks>;

        using waiting_queue_ref = Stage1DualPriorityQueueRef<RTTask*, n_tasks, smaller_ready_time<Clock>, smaller_deadline<Clock>>;
        using running_queue_ref = Stage2DualPriorityQueueRef<RTTask*, n_tasks, smaller_ready_time<Clock>, smaller_deadline<Clock>>;

        using waiting_queue_const_ref = Stage1DualPriorityQueueConstRef<RTTask*, n_tasks, smaller_ready_time<Clock>, smaller_deadline<Clock>>;
        using running_queue_const_ref = Stage2DualPriorityQueueConstRef<RTTask*, n_tasks, smaller_ready_time<Clock>, smaller_deadline<Clock>>;

        // TODO: dual priority queue with an unordered list to save more memory
        //       [stage 2 priority queue][reserve][unordered list][reserve][stage 1 priority list]
        //       consider if the complexity is worth it - probably not!
        dual_priority_queue_type _priority_queue; // ready + deadline
        ArrayList<RTTask*, n_tasks> _await_bench;
        unsigned int _next_task_id = 0; // next id for the next added task

    private:


        waiting_queue_ref waiting_queue(){return this->_priority_queue;}
        running_queue_ref running_queue(){return this->_priority_queue;}

        waiting_queue_const_ref waiting_queue() const {return this->_priority_queue;}
        running_queue_const_ref running_queue() const {return this->_priority_queue;}

        void promote(){
            // promote await back into running queue
            for(RTTask* task : this->_await_bench){
                if(!task->is_awaiting()){
                    logger::log_move(Clock::now(), task->name(), task->id(), "await", "run");
                    this->running_queue().push(task);
                }
            }
            this->_await_bench.erase_if([](const RTTask* task){return !task->is_awaiting();});

            // promote waiting queue into running queue
            if(!this->waiting_queue().empty()){
                while(!this->waiting_queue().empty()){
                    RTTask* task = this->waiting_queue().top();
                    const time_point now = Clock::now();
                    if(task->ready_time() <= now){
                        logger::log_move(now, task->name(), task->id(), "wait", "run");
                        this->waiting_queue().pop();
                        this->running_queue().push(task);
                    }else{
                        return;
                    }
                }
            }

            // do not check hibernation - hibernation will have to wake up by itself / external triggers
        }

        void run_next(){
            if(this->running_queue().empty()){
                FSleepUntil(this->waiting_queue().top()->ready_time());
            }else{
                RTTask* task = this->running_queue().top_pop();
                task->_execution_start = Clock::now();
                task->resume();
                logger::log_resume(task->_execution_start, Clock::now(), task->name(), task->id());
                // re-schedule
                
                const CoTaskSignal signal = task->get_signal();
                switch(signal.type()){
                    case CoTaskSignal::Type::Await : {
                        // clear signal
                        this->_await_bench.emplace_back(task);
                        logger::log_move(Clock::now(), task->name(), task->id(), "resume", "await");
                    }break;
                    case CoTaskSignal::Type::NextCycle : {
                        task->_schedule = task->next_schedule(task->_schedule, ExecutionTime<Clock>{task->_execution_start, Clock::now()});
                        this->waiting_queue().push(task);
                        logger::log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ImplicitDelay : {
                        const duration rel_deadline = task->_schedule.deadline - task->_schedule.ready;
                        task->_schedule.ready = Clock::now() + std::chrono::duration_cast<duration>(signal.implicit_delay().delay);
                        task->_schedule.deadline = task->_schedule.ready + rel_deadline;
                        this->waiting_queue().push(task);
                        logger::log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ExplicitDelay : {
                        task->_schedule.ready = Clock::now() + std::chrono::duration_cast<duration>(signal.explicit_delay().delay);
                        task->_schedule.deadline = task->_schedule.ready + std::chrono::duration_cast<duration>(signal.explicit_delay().rel_deadline);
                        logger::log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::None : {
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       logger::log_delete(Clock::now(), task->name(), task->id());
                    } break;
                    default : {
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       logger::log_delete(Clock::now(), task->name(), task->id());
                    } break;
                }
            }
        }

    public:

        void add(RealTimeTask<Clock>* task){
            task->id(this->_next_task_id++);

            const time_point now = Clock::now();
            if(task->ready_time() <= now){
                logger::log_add(now, task->name(), task->id(), "run");
                this->running_queue().push(task);
            }else{
                logger::log_add(now, task->name(), task->id(), "wait");
                this->waiting_queue().push(task);
            }
        }

        void spin(){
            this->promote();
            this->run_next();
        }

        /**
         * @brief returns the number of tasks currently in the waiting queue
         * 
         * Tasks that are in the waiting queue are waiting for time to pass until `Clock::now()`
         * is larger than the ready time of a task.
         */
        inline size_t n_waiting() const {
            return this->waiting_queue().size();
        }

        /**
         * @brief returns the number of tasks currently in the running queue
         * 
         * Tasks that are in the ready queue are all tasks that have a ready time that is larger
         * than `Clock::now()`.
         */
        inline size_t n_running() const {
            return this->running_queue().size();
        }

        /**
         * @brief returns the number of tasks currently in the awaiting queue
         * 
         * Tasks that are in the awaiting queue are waiting for a future or awaitable to become ready.
         */
        inline size_t n_awaiting() const {
            return this->_await_bench.size();
        }

        /**
         * @brief returns `true` if there are no tasks in its running queue
         */
        inline bool is_waiting() const {
            return this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are tasks in its running queue and `false` if the scheduler is waiting.
         */
        inline bool is_bussy() const {
            return !this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are no tasks in any queue
         */
        inline bool is_empty() const {
            return this->_priority_queue.empty() && this->_await_bench.empty();
        }

        /**
         * @brief returns `true` if there are no tasks in the sheduler
         */
        inline bool is_done() const {
            return this->_priority_queue.empty() && this->_await_bench.empty();
        }
    };
    
} // namespace embed
