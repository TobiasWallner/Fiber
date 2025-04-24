#pragma once

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

    template<CClock Clock>
    class ILogger{
    public:
        using cock = Clock;
        using time_point = typename Clock::time_point;
        using duration = typename Clock::duration;
        
        virtual inline ~ILogger(){}

        virtual inline void log_add(time_point time, const char* name, unsigned int id, const char* to_queue);
        virtual inline void log_move(time_point time, const char* name, unsigned int id, const char* from_queue, const char* to_queue);
        virtual inline void log_resume(time_point time_from, time_point time_to, const char* name, unsigned int id);
        virtual inline void log_remove(time_point time, const char* name, unsigned int id);
        virtual inline void log_sleep(time_point time, time_point sleep_until);
    };

    template<CClock Clock, size_t N>
    class Logger : public ILogger<Clock>{
    public:
        using cock = Clock;
        using time_point = typename Clock::time_point;
        using duration = typename Clock::duration;

        struct TaskAdd{time_point time; const char* name; unsigned int id; const char* to_queue;};
        struct TaskMove{time_point time; const char* name; unsigned int id; const char* from_queue; const char* to_queue;};
        struct TaskResume{time_point time_from; time_point time_to; const char* name; unsigned int id;};
        struct TaskRemove{time_point time; const char* name; unsigned int id;};
        struct Sleep{time_point time; time_point sleep_until;};

        using Event = typename std::variant<TaskAdd, TaskMove, TaskResume, TaskRemove, Sleep>;
    private:
        ArrayList<Event, N> _event_list;

    public:

        inline void log_add(time_point time, const char* name, unsigned int id, const char* to_queue) override {
            this->_event_list.emplace_back(TaskAdd{time, name, id, to_queue});
        }

        inline void log_move(time_point time, const char* name, unsigned int id, const char* from_queue, const char* to_queue) override {
            this->_event_list.emplace_back(TaskMove{time, name, id, from_queue, to_queue});
        }

        inline void log_resume(time_point time_from, time_point time_to, const char* name, unsigned int id) override {
            this->_event_list.emplace_back(TaskResume{time_from, time_to, name, id});
        }

        inline void log_remove(time_point time, const char* name, unsigned int id) override {
            this->_event_list.emplace_back(TaskRemove{time, name, id});
        }

        inline void log_sleep(time_point time, time_point sleep_until) override {
            // prevent log bloat
            // do not record the same sleep command multiple times.
            if(this->_event_list.empty()){
                this->_event_list.emplace_back(Sleep{time, sleep_until});
            }else(std::has_variant<Sleep>(this->_event_list.back())){
                if(std::get<Sleep>(this->_event_list.back()).sleep_until != sleep_until){
                    this->_event_list.emplace_back(Sleep{time, sleep_until});
                }
            }
        }
        
    };

    /**
     * @brief A real time scheduler that starts tasks once they are ready and schedules them by earliest deadline first
     * 
     * 
     * @tparam Clock The `embed::CClock` like clock that the scheduler should use. Defines the tick type, timer overflow, duration, time point and `now()` function.
     * @tparam n_tasks The maximum number of thats that will be pre-allocated for this scheduler.
     * @tparam FSleepUntil A function in the form `void sleep_until(Clock::time_point time)` that will be called if there is nothing to do and the MCU can enter sleep mode.
     * @tparam log A pointer to an output stream. If assigned the Scheduler will print logs. Logs contain: Added task, resumed task, task moved to different queue.
     * 
     */
    template<CClock Clock, size_t n_tasks, void (*FSleepUntil)(typename Clock::time_point) = default_sleep_until<Clock>, ILogger<Clock>* logger = nullptr>
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
        unsigned int _next_task_id; // next id for the next added task

    private:


        waiting_queue_ref waiting_queue(){return this->_priority_queue;}
        running_queue_ref running_queue(){return this->_priority_queue;}

        waiting_queue_const_ref waiting_queue() const {return this->_priority_queue;}
        running_queue_const_ref running_queue() const {return this->_priority_queue;}

        void promote(){
            // promote await back into running queue
            for(RTTask* task : this->_await_bench){
                if(!task->is_awaiting()){
                    if constexpr (logger != nullptr) logger->log_move(Clock::now(), task->name(), task->id(), "await", "run");
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
                        if constexpr (logger != nullptr) logger->log_move(now, task->name(), task->id(), "wait", "run");
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
                if constexpr (logger != nullptr) logger->log_resume(task->_execution_start, Clock::now(), task->name(), task->id());
                // re-schedule
                
                const CoTaskSignal signal = task->get_signal();
                switch(signal.type()){
                    case CoTaskSignal::Type::Await : {
                        // clear signal
                        this->_await_bench.emplace_back(task);
                        if constexpr (logger != nullptr) logger->log_move(Clock::now(), task->name(), task->id(), "resume", "await");
                    }break;
                    case CoTaskSignal::Type::NextCycle : {
                        task->_schedule = task->next_schedule(task->_schedule, ExecutionTime<Clock>{task->_execution_start, Clock::now()});
                        this->waiting_queue().push(task);
                        if constexpr (logger != nullptr) logger->log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ImplicitDelay : {
                        const duration rel_deadline = task->_schedule.deadline - task->_schedule.ready;
                        task->_schedule.ready = Clock::now() + std::chrono::duration_cast<duration>(signal.implicit_delay().delay);
                        task->_schedule.deadline = task->_schedule.ready + rel_deadline;
                        this->waiting_queue().push(task);
                        if constexpr (logger != nullptr) logger->log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ExplicitDelay : {
                        task->_schedule.ready = Clock::now() + std::chrono::duration_cast<duration>(signal.explicit_delay().delay);
                        task->_schedule.deadline = task->_schedule.ready + std::chrono::duration_cast<duration>(signal.explicit_delay().rel_deadline);
                        if constexpr (logger != nullptr) logger->log_move(Clock::now(), task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::None : {
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       if constexpr (logger != nullptr) logger->log_remove(Clock::now(), task->name(), task->id());
                    } break;
                    default : {
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       if constexpr (logger != nullptr) logger->log_remove(Clock::now(), task->name(), task->id());
                    } break;
                }
            }
        }

    public:

        void add(RealTimeTask<Clock>* task){
            task->id(this->_next_task_id++);

            const time_point now = Clock::now();
            if(task->ready_time() <= now){
                if constexpr (logger != nullptr) logger->log_add(now, task->name(), task->id(), "run");
                this->running_queue().push(task);
            }else{
                if constexpr (logger != nullptr) logger->log_add(now, task->name(), task->id(), "wait");
                this->waiting_queue().push(task);
            }
        }

        void spin(){
            this->promote();
            this->run_next();
        }

        /**
         * @brief returns `true` if there are no tasks in its running queue and the scheduler is waiting for a task to get ready.
         */
        bool is_waiting() const {
            return this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are tasks in its running queue and `false` if the scheduler is waiting.
         */
        bool is_bussy() const {
            return !this->running_queue().empty();
        }

        /**
         * @brief returns `true` if there are no tasks in any queue
         */
        bool is_empty() const {
            return this->_priority_queue.empty() && this->_await_bench.empty();
        }
    };
    
} // namespace embed
