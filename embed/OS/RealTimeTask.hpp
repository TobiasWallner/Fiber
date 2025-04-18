
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

    template<CClock Clock>
    class RealTimeTask : public CoTask{
    private:

        // TODO: store schedules in the scheduler - keep cache misses short.
        RealTimeSchedule<Clock> _schedule;
        Clock::time_point _execution_start;
    
    public:

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

        inline time_point ready_time() const {return this->_schedule.ready;}
        inline time_point deadline() const {return this->_schedule.deadline;}

        friend inline bool smaller_ready_time(const RealTimeTask& lhs, const RealTimeTask& rhs){
            return lhs.ready_time() < rhs.ready_time();
        }
    
        friend inline bool smaller_ready_time(const RealTimeTask& lhs, const time_point& rhs){
            return lhs.ready_time() < rhs;
        }

        friend inline bool smaller_deadline(const RealTimeTask& lhs, const RealTimeTask& rhs){
            return lhs.deadline() < rhs.deadline();
        }
    
        friend inline bool ready(const RealTimeTask& task){
            return task.ready_time() <= Clock::now();
        }
    };

} // namespace embed




    
