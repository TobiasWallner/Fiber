#pragma once

#include <embed/Chrono/TimePoint.hpp>
#include <embed/OS/RealTimeTask.hpp>

namespace embed{

    class PeriodicTask : public RealTimeTask{
    public:
        Duration _period;
        Duration _deadline;

    private:

        PeriodicTask(Coroutine<embed::Exit>&& main, std::string_view name, Duration period, Duration deadline, TimePoint first_ready)
            : RealTimeTask<Clock>(std::move(main), name, first_ready, deadline)
            , _period(period)
            , _deadline(deadline){}

        RealTimeSchedule<Clock> next_schedule(RealTimeSchedule schedule, [[maybe_unused]]ExecutionTime execution){
            schedule.ready += this->_period;
            schedule.deadline = schedule.ready + this->_deadline;
            return schedule;
        }

    };

    class SoftPeriodicTask : public RealTimeTask{
    public:

        TimePoint _prev_execution;
        Duration _period;
        Duration _deadline;
        Duration _offset = 0;

    private:

        PeriodicTask(Coroutine<embed::Exit>&& main, std::string_view name, Duration period, Duration deadline, TimePoint first_ready)
            : RealTimeTask<Clock>(std::move(main), name, first_ready, deadline)
            , _prev_execution(first_ready - period)
            , _period(period)
            , _deadline(deadline)
            {}

        RealTimeSchedule next_schedule(RealTimeSchedule schedule, ExecutionTime execution){
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


