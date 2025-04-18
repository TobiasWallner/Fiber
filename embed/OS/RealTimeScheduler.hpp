#pragma once

#include <embed/OS/Clock.hpp>
#include <embed/OS/Scheduler.hpp>
#include <embed/Containers/DualPriorityQueue.hpp>
#include <embed/OS/RealTimeTask.hpp>

namespace embed
{
    
    /**
     * @brief Default implementation for a function that should send the MCU to sleep until `time`, but does nothing.
     */
    template<CClock Clock>
    void default_sleep_until([[maybe_unused]]Clock::time_point time){
        return;
    }

    template<CClock Clock, size_t N>
    class Logger {
    public:
        struct TaskAdd{Clock::time_point time; const char* name; unsigned int id; const char* to_queue;};
        struct TaskMove{Clock::time_point time; const char* name; unsigned int id; const char* from_queue; const char* to_queue;};
        struct TaskResume{Clock::time_point time_from; Clock::time_point time_to; const char* name; unsigned int id};
        struct TaskRemove{Clock::time_point time; const char* name; unsigned int id;};
        struct Sleep{Clock::time_point time; Clock::time_point sleep_until;};

        using Event = typename std::variant<TaskAdd, TaskMove, TaskResume, TaskRemove, Sleep>;
    private:
        ArrayList<Event, N> _event_list;

    plublic:

        inline void log_add(Clock::time_point time, const char* name, unsigned int id, const char* to_queue){
            this->_event_list.emplace_back(TaskAdd{time, name, id, to_queue});
        }

        inline void log_move(Clock::time_point time, const char* name, unsigned int id, const char* from_queue, const char* to_queue){
            this->_event_list.emplace_back(TaskMove{time, name, id, from_queue, to_queue});
        }

        inline void log_resume(Clock::time_point time_from, Clock::time_point time_to, const char* name, unsigned int id){
            this->_event_list.emplace_back(TaskResume{time_from, time_to, name, id});
        }

        inline void log_remove(Clock::time_point time, const char* name, unsigned int id){
            this->_event_list.emplace_back(TaskRemove{time, name, id});
        }

        inline void log_sleep(Clock::time_point time, Clock::time_point sleep_until){
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
    template<CClock Clock, size_t n_tasks, class FSleepUntil = default_sleep_until, Logger* logger = nullptr>
    class RealTimeScheduler {
    private:
        using RTTask = typename RealTimeTask<Clock>;
        using dual_priority_queue_type = typename DualPriorityQueue<RTTask*, n_tasks, RTTask::smaller_ready_time, RTTask::smaller_deadline>;
        using dual_array_list_type = typename DualArrayList<RTTask*, n_tasks>;
        using waiting_queue_ref = typename Stage1DualPriorityQueue<RTTask*, n_tasks, RTTask::smaller_ready_time, RTTask::smaller_deadline>;
        using running_queue_ref = typename Stage2DualPriorityQueue<RTTask*, n_tasks, RTTask::smaller_ready_time, RTTask::smaller_deadline>;


        // TODO: dual priority queue with an unordered list to save more memory
        //       [stage 2 priority queue][reserve][unordered list][reserve][stage 1 priority list]
        //       consider if the complexity is worth it - probably not!
        dual_priority_queue_type _priority_queue; // ready + deadline
        ArrayList<CoTask*, n_background_tasks>; _await_bench;
        OStreamRef logging_stream; // stream for logging
        unsigned int _next_task_id; // next id for the next added task

    private:


        waiting_queue_type waiting_queue(){return this>_priority_queue;}
        running_queue_type running_queue(){return this>_priority_queue;}

        void promote(){
            // promote await back into running queue
            for(RTTask* task : this->_await_bench){
                if(!task->is_awaiting()){
                    if constexpr (logger) logger->log_move(Clock::now(), task->name(), task->id(), "await", "run");
                    this->running_queue().push(task);
                }
            }
            this->_await_bench.erase_if([](const RTTask* task){return !task->is_awaiting();})

            // promote waiting queue into running queue
            if(!this->waiting_queue().empty()){
                while(!this->waiting_queue().empty()){
                    RTTask* task = this->waiting_queue().top();
                    const time_point now = Clock::now();
                    if(task->ready_time() <= now){
                        if constexpr (logger) logger->log_move(now, task->name(), task->id(), "wait", "run");
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
            if(this->running_queue.empty()){
                FSleepUntil(this->waiting_queue().top().ready_time());
            }else{
                RTTask* task = this->running_queue().top_pop();
                task->_execution_start = Clock::now();;
                task->resume();
                if constexpr (logger) logger->log_resume(task->_execution_start, Clock::now();, task->name(), task->id());
                // re-schedule
                
                const CoTaskSignal signal = task->get_signal();
                switch(signal){
                    case CoTaskSignal::Type::Await : {
                        // clear signal
                        this->_await_bench.emplace_back(task);
                        if constexpr (logger) logger->log_move(Clock::now();, task->name(), task->id(), "resume", "await");
                    }break;
                    case CoTaskSignal::Type::NextCycle : {
                        task->_schedule = task->next_schedule(task->_schedule, ExecutionTime<Clock>{task->_execution_start, Clock::now()});
                        this->waiting_queue.push(task);
                        if constexpr (logger) logger->log_move(Clock::now();, task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ImplicitDelay : {
                        const duration rel_deadline = task->_schedule.deadline - task->_schedule.ready;
                        task->_schedule.ready = Clock::now() + signal.implicit_delay();
                        task->_schedule.deadline = task->_schedule.ready + rel_deadline;
                        this->waiting_queue.push(task);
                        if constexpr (logger) logger->log_move(Clock::now();, task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::ExplicitDelay : {
                        task->_schedule.ready = Clock::now() + signal.explicit_delay().delay;
                        task->_schedule.deadline = task->_schedule.ready + signal.explicit_delay().rel_deadline;
                        if constexpr (logger) logger->log_move(Clock::now();, task->name(), task->id(), "resume", "wait");
                    }break;
                    case CoTaskSignal::Type::None : {
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       if constexpr (logger) logger->log_remove(Clock::now();, task->name(), task->id());
                    } break;
                    default : {
                        task->signal().none();
                        /* 
                        - Nothing to do 
                        - task probably finished 
                        - let it die by not inserting it into lists
                        */
                       if constexpr (logger) logger->log_remove(Clock::now(), task->name(), task->id());
                    } break;
                }

                
            }
        }

    public:

        void addTask(RealTimeTask* task){
            this->task->set_id(this->_next_task_id++);

            const time_point now = Clock::now();
            if(this->task->ready_time <= now){
                if constexpr (logger) logger->log_add(now, task->name(), task->id(), "run"){
                this->running_queue().push(task);
            }else{
                if constexpr (logger) logger->log_add(now, task->name(), task->id(), "wait"){
                this->waiting_queue().push(task);
            }

        }

        void spin(){
            this->promote();
            this->run_next();
        }


    };
    
} // namespace embed
