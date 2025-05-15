
#pragma once

#include <fiber/Chrono/TimePoint.hpp>
#include <fiber/OS/Coroutine.hpp>

namespace fiber
{

    struct RealTimeSchedule{
        TimePoint ready;
        TimePoint deadline;
    };

    struct ExecutionTime{
        TimePoint start;
        TimePoint end;
    };

    /**
     * \brief A real time task is time aware. It has an assigned clock, a ready time and a deadline.
     * 
     * Real-time tasks can be used in real-time schedulers
     * The real-time schedulers derives from `Task` and additionally provides a ready-time.
     * The RealTimeScheduler will only start the task after the ready time.
     * Further, it provides a deadline. A real-time scheduler will prioritise task with sooner deadlines.
     * 
     * In case you like to create your own real-time task and derive from this one, it provides the following customisations:
     * 
     * - Overload the following to calculate the next schedule from the last schedule and/or execution times.
     *   ```cpp
     *   virtual RealTimeSchedule next_schedule(RealTimeSchedule previous_schedule, ExecutionTime previous_execution)
     *   ``` 
     *   
     * - Overload the following to decide weather or not a missed deadline should or should not resume the task, given the duration that passed since the deadline.
     *   ```
     *   virtual bool missed_deadline(fiber::Duration d)
     *   ```
     *   
     * Methods that you might like to overload from `Task`
     * 
     * - Overload the following to decide what happens on uncaught exceptions in coroutines.
     *   ```
     *   #ifndef FIBER_DISABLE_EXCEPTIONS
     *       // variation using exceptions
     *       void handle_exception(std::exception_ptr except_ptr);
     *   #else
     *       // variation limiting the use of exceptions
     *       void handle_exception();
     *   #endif
     *   ```
     * 
     * \see fiber::Task
     * \see fiber::RealTimeScheduler
     * \see fiber::CClock
     * \see fiber::Clock
     * \see fiber::ClockTick
     */
    class RealTimeTask : public Task{
    private:
    public:
        // TODO: store schedules in the scheduler - keep cache misses short - remove from user perspective
        RealTimeSchedule _schedule;
        TimePoint _execution_start;

        /**
         * @brief sets the ready time to the absolute time point and the deadline relative from the start time
         */
        inline RealTimeTask(Coroutine<fiber::Exit>&& main, std::string_view name, fiber::TimePoint ready, fiber::Duration deadline) 
            : Task(std::move(main), name)
        {
            this->_schedule.ready = ready;
            this->_schedule.deadline = this->_schedule.ready + deadline;
        }

        template<class Rep, CRatio Period>
        inline RealTimeTask(Coroutine<fiber::Exit>&& main, std::string_view name, fiber::TimePoint ready, std::chrono::duration<Rep, Period> deadline) 
            : RealTimeTask(std::move(main), name, ready, fiber::rounding_duration_cast<Duration>(deadline)){}

        /**
         * @brief sets the ready and deadline time to the absolute time points
         */
        RealTimeTask(Coroutine<fiber::Exit>&& main, std::string_view name, fiber::TimePoint ready, fiber::TimePoint deadline) 
            : 
        Task(std::move(main), name){
            this->_schedule.ready = ready;
            this->_schedule.deadline = deadline;
        }

        /**
         * @brief Overrideable: Gets called after a `co_await NextCycle;` to calculate the schedule of the next cycle.
         * 
         * Note that `previous_execution.end` is equivalent to the current time .aka `now()`
         * 
         * @param previous_schedule the previous schedule of this task
         * @param previous_execution the previous execution time from the start of the cycle to the end of the cycle
         */
        virtual RealTimeSchedule next_schedule(
            [[maybe_unused]]RealTimeSchedule previous_schedule, 
            [[maybe_unused]]ExecutionTime previous_execution)
        {
            return RealTimeSchedule{previous_execution.end, previous_execution.end};
        }

        /**
         * @brief Overrideable: Gets called if the deadline has been missed and decides wether the task should still execute or not
         * @param d The duration/time that passed since the deadline
         * @details The default version will always return `true` to continue the task
         * @returns Returns if the task should still be executed (`true`) or not (`false`)
         */
        virtual bool missed_deadline([[maybe_unused]]fiber::Duration d){return true;}

        /**
         * \brief returns the time point at which this task becomes ready to be executed
         */
        inline fiber::TimePoint ready_time() const {return this->_schedule.ready;}

        /**
         * \brief returns the time point before which this task needs to be executed
         */
        inline fiber::TimePoint deadline() const {return this->_schedule.deadline;}

        
    };

    struct smaller_ready_time{
        inline bool operator() (const RealTimeTask& lhs, const RealTimeTask& rhs){return lhs.ready_time() < rhs.ready_time();}
        inline bool operator() (const RealTimeTask* lhs, const RealTimeTask* rhs){return lhs->ready_time() < rhs->ready_time();}
        inline bool operator() (const RealTimeTask& lhs, const typename fiber::TimePoint& rhs){return lhs.ready_time() < rhs;}
        inline bool operator() (const RealTimeTask* lhs, const typename fiber::TimePoint& rhs){return lhs->ready_time() < rhs;}
    };

    struct larger_ready_time{
        inline bool operator() (const RealTimeTask& lhs, const RealTimeTask& rhs){return lhs.ready_time() > rhs.ready_time();}
        inline bool operator() (const RealTimeTask* lhs, const RealTimeTask* rhs){return lhs->ready_time() > rhs->ready_time();}
        inline bool operator() (const RealTimeTask& lhs, const typename fiber::TimePoint& rhs){return lhs.ready_time() > rhs;}
        inline bool operator() (const RealTimeTask* lhs, const typename fiber::TimePoint& rhs){return lhs->ready_time() > rhs;}
    };

    struct smaller_deadline{
        inline bool operator() (const RealTimeTask& lhs, const RealTimeTask& rhs){return lhs.deadline() < rhs.deadline();}
        inline bool operator() (const RealTimeTask* lhs, const RealTimeTask* rhs){return lhs->deadline() < rhs->deadline();}
        inline bool operator() (const RealTimeTask& lhs, const typename fiber::TimePoint& rhs){return lhs.deadline() < rhs;}
        inline bool operator() (const RealTimeTask* lhs, const typename fiber::TimePoint& rhs){return lhs->deadline() < rhs;}
    };

    struct larger_deadline{
        inline bool operator() (const RealTimeTask& lhs, const RealTimeTask& rhs){return lhs.deadline() > rhs.deadline();}
        inline bool operator() (const RealTimeTask* lhs, const RealTimeTask* rhs){return lhs->deadline() > rhs->deadline();}
        inline bool operator() (const RealTimeTask& lhs, const typename fiber::TimePoint& rhs){return lhs.deadline() > rhs;}
        inline bool operator() (const RealTimeTask* lhs, const typename fiber::TimePoint& rhs){return lhs->deadline() > rhs;}
    };

} // namespace fiber




    
