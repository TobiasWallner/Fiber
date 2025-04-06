#pragma once

namespace embed
{
    /**
     * @brief Task that will periodically execute with a fixed period.
     * 
     * Hard in this context means, that the planned absolute start time is set in stone.
     * The task will be scheduled every `n * period + t0` in absolute time and does not allow
     * that its phase or its start time within the period cycle changes.
     * 
     * When would you choose this periodic task type?
     * ----
     * Choose this task if:
     *   - timeing accuracy (distance to start of counting) is more important than timeing precision (distance to previous execution).
     *   - you have a fixed schedule and time slots
     *   - the tasks execution window/timeslot within the perod cycle is not allowed to shift or drift to fit scheduling
     * 
     * When shouldn't you choose this task type?
     * ----
     * Do not choose this task if:
     *   - timeing precision is more important than timeing accuracy.
     *   - the tasks execution window within the period cycle is allowed to shift or drift, to fit sheduler needs.
     * In that case, see SoftPeriodicTask 
     * 
     */
    class HardPeriodicTask : public BaseTask{
        std::chrono::nanoseconds _period;
        std::chrono::nanoseconds _deadline_window;
        std::chrono::nanoseconds _offset;
    public:

        inline HardPeriodicTask(Coroutine&& main, std::chrono::nanoseconds period, std::chrono::nanoseconds deadline_window, std::chrono::nanoseconds offset)
            : BaseTask(std::move(main))
            , _period(period)
            , _deadline_window(deadline_window)
            , _offset(offset){}
        

        /**
         * @brief Initialises the first schedule
         * 
         * Gets called by the scheduler when the task is passed to the scheduler the first time.
         * The scheduler passes its current time and expects a Schedule in return
         * 
         * @param now The current time as seen by the scheduler
         * 
         * @returns The first Schedule in which the task needs to be executed
         */
        Schedule init_schedule(std::chrono::nanoseconds now) {
            const std::chrono::nanoseconds planned_start = now + this->_offset;
            const std::chrono::nanoseconds planned_deadline = planned_start + this->_deadline_window;
            return Schedule{.ready = planned_start, .deadline = planned_deadline};
        }

        /**
         * @brief Calculates the next schedule
         * 
         * Gets called by the scheduler after a cycle ends. 
         * A cycle ends when the co-routine returns with:
         * 
         * ```cpp
         * co_yield embed::cycle_end();
         * ```
         * 
         * The scheduler passes the last palanned schedule and the execution time of the currently finished cycle
         * and expects a new schedule in return.
         * 
         * @param last_schedule The `Schedule` of the previous execution cycle
         * @param execution_times The measured `ExecutionTimes` of the just finished cycle
         * 
         * @returns The `Schedule` of the next cycle
         */
        Schedule update_schedule(const Schedule& last_schedule, [[maybe_unused]]const ExecutionTimes& execution_times) {
            const std::chrono::nanoseconds planned_start = last_schedule.ready + this->_period;
            const std::chrono::nanoseconds planned_deadline = last_schedule.deadline + this->_period;
            return Schedule{.ready = planned_start, .deadline = planned_deadline};
        }

        
    };
} // namespace embed


        