#pragma once

#include <embed/OS/Clock.hpp>
#include <embed/OS/RealTimeTask.hpp>


/*

TODO: Make a periodic task

TODO: 
- a page that lists available the schedulers 
- a page that lists available tasks, what they are for, and which schedulers they can be used with

*/

namespace embed{
    
    template<CClock Clock>
    class PeriodicTask : public RealTimeTask<Clock>{
    public:

        using clock = Clock;
        using time_point = typename Clock::time_point
        using duration = typename Clock::duration;

        duration _period;
        duration _deadline;

    private:

        PeriodicTask(Coroutine<embed::Exit>&& main, std::string_view name, duration period, duration deadline, time_point first_ready = Clock::now())
            : RealTimeTask<Clock>(std::move(main), name, first_ready, deadline)
            , _period(period)
            , _deadline(deadline){}

        RealTimeSchedule<Clock> next_schedule(RealTimeSchedule<Clock> schedule, [[maybe_unused]]ExecutionTime<Clock> execution){
            schedule.ready += this->_period;
            schedule.deadline = schedule.ready + this->_deadline;
            return schedule;
        }

    };

    template<CClock Clock>
    class SoftPeriodicTask : public RealTimeTask<Clock>{
    public:

        using clock = Clock;
        using time_point = typename Clock::time_point
        using duration = typename Clock::duration;

        time_point _prev_execution;
        duration _period;
        duration _deadline;
        duration _offset = 0;

    private:

        PeriodicTask(Coroutine<embed::Exit>&& main, std::string_view name, duration period, duration deadline, time_point first_ready = Clock::now())
            : RealTimeTask<Clock>(std::move(main), name, first_ready, deadline)
            , _prev_execution(first_ready - period)
            , _period(period)
            , _deadline(deadline)
            {}

        RealTimeSchedule<Clock> next_schedule(RealTimeSchedule<Clock> schedule, ExecutionTime<Clock> execution){
            const duration measured_period = execution.start - _prev_execution;
            const duration error = this->_period - measured_period;
            this->_offset += error / 32;
            RealTimeSchedule<Clock> new_schedule;
            new_schedule.ready = execution.start + this->_period + this->_offset;
            new_schedule.deadline = new_schedule.ready + this->_deadline + this->_offset;
            return new_schedule
        }

    };

} // namespace embed


