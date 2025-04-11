#pragma once

//std
#include <coroutine>
#include <chrono>

//embed
#include "embed/OStream.hpp"
#include "embed/Future.hpp"
#include "embed/OS/Exit.hpp"
#include "embed/OS/TaskFuture.hpp"
#include "embed/OS/Schedule.hpp"
#include "embed/OS/ExecutionTimes.hpp"
#include "embed/OS/TaskLifeCycle.hpp"
#include "embed/OS/TaskFuture.hpp"
//#include "embed/OS/Clock.hpp"

namespace embed{

        // foreward declarations for friends
        class BaseTask_TestSuit;
        class OS;

        class BaseTask{
        public:
            using Coroutine = embed::TaskFuture<embed::Exit>;
            friend class OS;
            friend class BaseTask_TestSuit;
        private:
            Coroutine _coroutine;
            Schedule _schedule;
            TimePoint _delayed_resume_time = 0ns; // next re-redy after yield via delay
            TimePoint _execution_start_time = 0ns; // scheduled start time after which task execution is allowed

            TaskLifeCycle _prev_life_cycle = TaskLifeCycle::New;
            TaskLifeCycle _life_cycle = TaskLifeCycle::New;
        public:

            BaseTask(Coroutine&& main) : _coroutine(std::move(main)){}
        private:
            inline void resume(){this->_coroutine.resume();}
        public:
            [[nodiscard]] inline bool done() const {return this->_coroutine.done();}
            
            [[nodiscard]] inline TaskLifeCycle life_cycle() const {return this->_life_cycle;}
            [[nodiscard]] inline TaskLifeCycle previous_life_cycle() const {return this->_prev_life_cycle;}
        private:            
            inline void life_cycle(TaskLifeCycle value) {
                this->_prev_life_cycle = this->_life_cycle;
                this->_life_cycle = value;
            }
        public:

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
            virtual Schedule init_schedule(TimePoint now) = 0;

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
             * The function has to be overridded by the deriving task to calculate the next schedule.
             * 
             * @param last_schedule The `Schedule` of the previous execution cycle
             * @param execution_times The measured `ExecutionTimes` of the just finished cycle
             * 
             * @returns The `Schedule` of the next cycle
             */
            virtual Schedule update_schedule(const Schedule& last_schedule, const ExecutionTimes& execution_times) = 0;

            /**
             * @brief A function that can be overloaded by the user and will be called by the
             * scheduler, if the deadline has been missed.
             * 
             * @param over_due the time that the task is over due (aka. the time distance `now - deadline`)
             * 
             * @returns 
             *  - `true` -> the scheduler will still execute the task. 
             *  - `false` -> the scheduler will not execute the task and call `update_schedule()` for the next cycle, 
             *    If `on_deadline_miss()` has not killed the task, in which case `on_kill()`
             */
            virtual bool on_deadline_miss(Duration over_due) const {return true;}

            /// @brief A function that can be overloaded by the user and will be called by the scheduler
            /// if the task has been killed. This allows for extra cleanup.
            virtual void on_kill() const {/* default: RIP */};
        
            /// @brief returns the current schedule
            [[nodiscard]] inline Schedule schedule() const {return this->_schedule;}

            /// @brief returns the time after which the task is ready next.
            /// @details if the task is in its Delaying lifecycle it will return the delayed resume time, otherwise the ready time of the schedule 
            [[nodiscard]] inline TimePoint ready_time() const {
                if(this->life_cycle() == TaskLifeCycle::Delaying){
                    return this->_delayed_resume_time;
                }else{
                    return this->_schedule.ready;
                }
            }

            /// @brief returns a CoroutineStatusType the current status of the coroutine
            [[nodiscard]] inline CoroutineStatusType co_return_type() const {return this->_coroutine.co_return_type();}
            
            /// @brief returns true if the coroutine returned via yielding. `co_yield;`
            [[nodiscard]] inline bool is_yielding() const {return this->_coroutine.is_yielding();}

            /// @brief returns true if the coroutine returned by ending a cycle. `co_yield embed::Cycle();`
            [[nodiscard]] inline bool is_ending_cycle() const {return this->_coroutine.is_ending_cycle();}

            /// @brief returns true if the coroutine returned via a delay. `co_yield 10ms`.
            [[nodiscard]] inline bool is_delaying() const {return this->_coroutine.is_delaying();}

            /// @brief returns true if the coroutine is awaiting on Future data. `data = co_await get_async_data();` 
            [[nodiscard]] inline bool is_awaiting() const {return this->_coroutine.is_awaiting();}

            /// @brief returns true if the coroutine has returned. `co_return Exit::Success;` 
            [[nodiscard]] inline bool is_returning() const {return this->_coroutine.is_returning();}            

            /**
             * @brief returns the return value of the task
             * @throws An O1 level assertion if the task is not actually returning, aka. `this->is_returning(); // false`
             * @returns the return value of the task
             *  */ 
            [[nodiscard]] inline Exit return_value() const {
                EMBED_ASSERT_O1(this->is_returning());
                return this->_coroutine.get_return_value();
            }

            /**
             * @brief return the delay passed to `co_yield`
             * @throws An O1 level assertion if the task is not actually delaying, aka. `this->is_delaying(); // false`
             * @returns the delay
             */
            [[nodiscard]] inline Duration delay_value() const {
                EMBED_ASSERT_O1(this->is_delaying());
                return this->_coroutine.get_delay_value();
            }

        };

        template<TimePoint modulo>
        inline Duration unwrapped_distance(TimePoint lhs, TimePoint rhs){
            const Duration result = (t_lhs < t_rhs) ? (t_rhs - t_lhs) : ((t_rhs + modulo) - t_lhs);
            return result;
        }
        
        template<TimePoint modulo>
        inline bool is_earlier(TimePoint lhs, TimePoint rhs){
            const Duration distance = unwrapped_distance<modulo>(lhs, rhs);
            const bool result = distance < (max_time / 2);
            return result;
        }

        template<TimePoint modulo>
        inline bool has_earlier_start_time(const BaseTask* lhs, const BaseTask* rhs){
            return is_earlier<modulo>(lhs->schedule().ready, rhs->schedule().ready);
        }

        template<TimePoint modulo>
        inline bool has_earlier_deadline(const BaseTask* lhs, const BaseTask* rhs){
            return is_earlier<modulo>(lhs->schedule().deadline, rhs->schedule().deadline);
        }

}//embed

