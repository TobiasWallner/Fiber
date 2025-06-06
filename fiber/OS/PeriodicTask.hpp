#pragma once

#include <fiber/Chrono/TimePoint.hpp>
#include <fiber/OS/Task.hpp>

namespace fiber{

    class PeriodicTask : public Task{
    public:
        Duration _period;
        Duration _deadline;

    private:

        PeriodicTask(Coroutine<fiber::Exit>&& main, std::string_view name, Duration period, Duration deadline, TimePoint first_ready)
            : Task<Clock>(std::move(main), name, first_ready, deadline)
            , _period(period)
            , _deadline(deadline){}

        RealTimeSchedule<Clock> next_schedule(RealTimeSchedule schedule, [[maybe_unused]]ExecutionTime execution){
            schedule.ready += this->_period;
            schedule.deadline = schedule.ready + this->_deadline;
            return schedule;
        }

    };

    class SoftPeriodicTask : public Task{
    public:

        TimePoint _prev_execution;
        Duration _period;
        Duration _deadline;
        Duration _offset = 0;

    private:

        PeriodicTask(Coroutine<fiber::Exit>&& main, std::string_view name, Duration period, Duration deadline, TimePoint first_ready)
            : Task<Clock>(std::move(main), name, first_ready, deadline)
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

} // namespace fiber


