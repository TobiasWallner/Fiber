
#pragma once

#include <embed/OS/Clock.hpp>
#include <embed/OS/Coroutine.hpp>

namespace embed
{


    template<CClock Clock>
    struct RealTimeSchedule{
        Clock::time_point ready;
        Clock::time_point deadline;
    };

    template<CClock Clock>
    struct ExecutionTime{
        Clock::time_point start;
        Clock::time_point end;
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
     *   virtual RealTimeSchedule<Clock> next_schedule(RealTimeSchedule<Clock> previous_schedule, ExecutionTime<Clock> previous_execution)
     *   ``` 
     *   
     * - Overload the following to decide weather or not a missed deadline should or should not resume the task, given the duration that passed since the deadline.
     *   ```
     *   virtual bool missed_deadline(Clock::duration d)
     *   ```
     *   
     * Methods that you might like to overload from `Task`
     * 
     * - Overload the following to decide what happens on uncaught exceptions in coroutines.
     *   ```
     *   
     *   ```
     * 
     * \tparam Clock A Clock type that comforms to the template `embed::CClock`.
     * 
     * \see embed::Task
     * \see embed::RealTimeScheduler
     * \see embed::CClock
     * \see embed::Clock
     * \see embed::ClockTick
     */
    template<CClock Clock>
    class RealTimeTask : public Task{
    private:
    public:

        // TODO: store schedules in the scheduler - keep cache misses short - remove from user perspective
        RealTimeSchedule<Clock> _schedule;
        Clock::time_point _execution_start;
    

        using clock = Clock;
        using duration = Clock::duration;
        using time_point = Clock::time_point;

        /**
         * @brief sets the ready time relative from now and the deadline relative from the start time
         */
        RealTimeTask(Coroutine<embed::Exit>&& main, std::string_view name, duration ready, duration deadline) 
            : Task(std::move(main), name)
        {
            this->_schedule.ready = Clock::now() + ready;
            this->_schedule.deadline = this->_schedule.ready + deadline;
        }

        /**
         * @brief sets the ready time to the absolute time point and the deadline relative from the start time
         */
        RealTimeTask(Coroutine<embed::Exit>&& main, std::string_view name, time_point ready, duration deadline) : Task(std::move(main), name){
            this->_schedule.ready = ready;
            this->_schedule.deadline = this->_schedule.ready + deadline;
        }

        /**
         * @brief sets the ready and deadline time to the absolute time points
         */
        RealTimeTask(Coroutine<embed::Exit>&& main, std::string_view name, time_point ready, time_point deadline) : Task(std::move(main), name){
            this->_schedule.ready = ready;
            this->_schedule.deadline = deadline;
        }

        /**
         * @brief Overrideable: Gets called after a `co_await NextCycle;` to calculate the schedule of the next cycle.
         * @param previous_schedule the previous schedule of this task
         * @param previous_execution the previous execution time from the start of the cycle to the end of the cycle
         */
        virtual RealTimeSchedule<Clock> next_schedule(
            [[maybe_unused]]RealTimeSchedule<Clock> previous_schedule, 
            [[maybe_unused]]ExecutionTime<Clock> previous_execution)
        {
            return RealTimeSchedule<Clock>{Clock::now(), Clock::now()};
        }

        /**
         * @brief Overrideable: Gets called if the deadline has been missed and decides wether the task should still execute or not
         * @param d The duration/time that passed since the deadline
         * @details The default version will always return `true` to continue the task
         * @returns Returns if the task should still be executed (`true`) or not (`false`)
         */
        virtual bool missed_deadline([[maybe_unused]]Clock::duration d){return true;}

        /**
         * \brief returns the time point at which this task becomes ready to be executed
         */
        inline time_point ready_time() const {return this->_schedule.ready;}

        /**
         * \brief returns the time point before which this task needs to be executed
         */
        inline time_point deadline() const {return this->_schedule.deadline;}

        
    };

    template<CClock Clock>
    struct smaller_ready_time{
        inline bool operator() (const RealTimeTask<Clock>& lhs, const RealTimeTask<Clock>& rhs){return lhs.ready_time() < rhs.ready_time();}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const RealTimeTask<Clock>* rhs){return lhs->ready_time() < rhs->ready_time();}
        inline bool operator() (const RealTimeTask<Clock>& lhs, const typename Clock::time_point& rhs){return lhs.ready_time() < rhs;}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const typename Clock::time_point& rhs){return lhs->ready_time() < rhs;}
    };

    template<CClock Clock>
    struct larger_ready_time{
        inline bool operator() (const RealTimeTask<Clock>& lhs, const RealTimeTask<Clock>& rhs){return lhs.ready_time() > rhs.ready_time();}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const RealTimeTask<Clock>* rhs){return lhs->ready_time() > rhs->ready_time();}
        inline bool operator() (const RealTimeTask<Clock>& lhs, const typename Clock::time_point& rhs){return lhs.ready_time() > rhs;}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const typename Clock::time_point& rhs){return lhs->ready_time() > rhs;}
    };

    template<CClock Clock>
    struct smaller_deadline{
        inline bool operator() (const RealTimeTask<Clock>& lhs, const RealTimeTask<Clock>& rhs){return lhs.deadline() < rhs.deadline();}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const RealTimeTask<Clock>* rhs){return lhs->deadline() < rhs->deadline();}
        inline bool operator() (const RealTimeTask<Clock>& lhs, const typename Clock::time_point& rhs){return lhs.deadline() < rhs;}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const typename Clock::time_point& rhs){return lhs->deadline() < rhs;}
    };

    template<CClock Clock>
    struct larger_deadline{
        inline bool operator() (const RealTimeTask<Clock>& lhs, const RealTimeTask<Clock>& rhs){return lhs.deadline() > rhs.deadline();}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const RealTimeTask<Clock>* rhs){return lhs->deadline() > rhs->deadline();}
        inline bool operator() (const RealTimeTask<Clock>& lhs, const typename Clock::time_point& rhs){return lhs.deadline() > rhs;}
        inline bool operator() (const RealTimeTask<Clock>* lhs, const typename Clock::time_point& rhs){return lhs->deadline() > rhs;}
    };

    template<CClock Clock>
    inline bool is_ready(const RealTimeTask<Clock>& task){return task.ready_time() <= Clock::now();}

    template<CClock Clock>
    inline bool is_ready(const RealTimeTask<Clock>* task){return task->ready_time() <= Clock::now();}

} // namespace embed




    
