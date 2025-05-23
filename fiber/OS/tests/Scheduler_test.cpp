#include "Scheduler_test.hpp"

#include <fiber/TestFramework/TestFramework.hpp>
#include <fiber/Chrono/TimePoint.hpp>
#include <fiber/OS/Task.hpp>
#include <fiber/OS/Scheduler.hpp>
#include <fiber/Memory/StaticLinearAllocator.hpp>
#include <fiber/OS/Delay.hpp>

namespace fiber
{
    namespace
    {
        TimePoint g_mock_time(0);
        TimePoint get_time(){return g_mock_time;}

        TestResult one_task_immediatelly_ready_finishes_instantly(){
            TEST_START;

            g_mock_time = TimePoint(0);

            class CoSuit{
                public:
                int proof = 0;
                static Coroutine<Exit> coroutine(CoSuit& self){
                    self.proof = 258;
                    co_return Exit::Success;
                }
            };

            CoSuit co_suit;

            Task<512> simpleTask("simpleTask", get_time(), 1ms, CoSuit::coroutine, co_suit);

            Scheduler<1> scheduler(get_time);

            TEST_TRUE(scheduler.is_waiting());
            TEST_FALSE(simpleTask.is_done());

            scheduler.add(&simpleTask);

            TEST_FALSE(scheduler.is_waiting());
            TEST_FALSE(simpleTask.is_done());
            TEST_EQUAL(co_suit.proof, 0);

            scheduler.spin();

            TEST_TRUE(scheduler.is_waiting());
            TEST_TRUE(scheduler.is_empty());
            TEST_TRUE(simpleTask.is_done());
            TEST_EQUAL(co_suit.proof, 258);

            TEST_END;
        }

        TestResult one_task_delayed_ready_finishes_instantly(){
            TEST_START;

            // global state setup
            g_mock_time = TimePoint(0);
            
            // task setup
            class Task : public fiber::Task<256>{
                public:
                int proof = 0;

                Task(std::string_view name, TimePoint ready, Duration deadline) 
                    : fiber::Task<256>(name, ready, deadline, Task::main, this){}

                Task(std::string_view name, TimePoint ready, std::chrono::milliseconds deadline) 
                    : fiber::Task<256>(name, ready, deadline, Task::main, this){}

                static Coroutine<Exit> main(Task* This){
                    This->proof = 258;
                    co_return Exit::Success;
                }
            };

            Task simpleTask("simpleTask", get_time() + 1ms, 2ms);

            Scheduler<1> scheduler(get_time);

            // added task is not done
            scheduler.add(&simpleTask);

            TEST_TRUE(scheduler.is_waiting());
            TEST_FALSE(simpleTask.is_done());
            TEST_EQUAL(simpleTask.proof, 0);

            // spinning does not help because the time is not right yet
            scheduler.spin();

            TEST_TRUE(scheduler.is_waiting());
            TEST_FALSE(simpleTask.is_done());
            TEST_EQUAL(simpleTask.proof, 0);

            // only after time passes the task becomes ready and executed
            g_mock_time += 1ms;
            scheduler.spin();

            TEST_TRUE(scheduler.is_waiting());
            TEST_TRUE(scheduler.is_empty());
            TEST_TRUE(simpleTask.is_done());
            TEST_EQUAL(simpleTask.proof, 258);

            TEST_END;
        }

    TestResult one_task_immediatelly_ready_delays(){
        TEST_START;

        // global state setup
        g_mock_time = TimePoint(0);

        // task setup
        class Task : public fiber::Task<256>{
            public:
            int proof = 0;

            Task(std::string_view name, Duration ready, Duration deadline) 
                : fiber::Task<256>(name, ready + get_time(), deadline, Task::main, this){}

            Task(std::string_view name, std::chrono::milliseconds ready, std::chrono::milliseconds deadline) 
                : Task(name, fiber::rounding_duration_cast<fiber::Duration>(ready), fiber::rounding_duration_cast<fiber::Duration>(deadline)){}

            static Coroutine<Exit> main(Task* This){
                This->proof = 1;
                co_await Delay(2ms);
                This->proof = 2;
                co_return Exit::Success;
            }
        };

        Task task("Task", 1ms, 2ms);

        // OutputLogger<MockClock>::stream = fiber::cout;
        // Scheduler<MockClock, 1, fiber::default_sleep_until<MockClock>, OutputLogger<MockClock>> scheduler;

        Scheduler<1> scheduler(get_time);

        // added task is not done
        scheduler.add(&task);

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 0);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = TimePoint(0);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 0);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = TimePoint(1ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 1);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = TimePoint(2ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 1);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = TimePoint(3ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 0);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 2);
        TEST_TRUE(scheduler.is_done());
        TEST_TRUE(task.is_done());

        TEST_END;
    }


    TestResult two_tasks_first_has_lower_ready_second_has_lower_deadline(){
        TEST_START;

        // global state setup
        g_mock_time = TimePoint(0);

        // task setup
        class Task : public fiber::Task<256>{
            /*
            Tests the Earliest-Ready-Time / Earliest-Deadline scheduling priorities
             */
            public:
            int proof = 0;

            Task(std::string_view name, TimePoint ready, Duration deadline) 
                : fiber::Task<256>(name, ready, deadline, Task::main, this){}

            Task(std::string_view name, TimePoint ready, std::chrono::milliseconds deadline) 
                : fiber::Task<256>(name, ready, deadline, Task::main, this){}

            static Coroutine<Exit> main(Task* This){
                This->proof = 1;
                co_await Delay(0ms); // basically yield to other tasks
                This->proof = 2;
                co_return Exit::Success;
            }
        };

        Task task1("Task 1", get_time() + 1ms, 4ms);
        Task task2("Task two", get_time() + 2ms, 2ms);

        // OutputLogger<MockClock>::stream = fiber::cout;
        // Scheduler<MockClock, 2, fiber::default_sleep_until<MockClock>, OutputLogger<MockClock>> scheduler;
        Scheduler<2> scheduler(get_time);

        // added task is not done
        scheduler.add(&task1);

        scheduler.add(&task2);

        TEST_EQUAL(scheduler.n_waiting(), 2);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_TRUE(scheduler.is_waiting());
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(scheduler.is_busy());
        TEST_TRUE(scheduler.is_full());
        TEST_NOT_EQUAL(task1.id(), task2.id())
        TEST_FALSE(task1.is_done());
        TEST_EQUAL(task1.proof, 0);
        TEST_FALSE(task2.is_done());
        TEST_EQUAL(task2.proof, 0);

        g_mock_time = TimePoint(0);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 2);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_TRUE(scheduler.is_waiting());
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(scheduler.is_busy());
        TEST_TRUE(scheduler.is_full());
        TEST_NOT_EQUAL(task1.id(), task2.id()); // unique id's
        TEST_FALSE(task1.is_done());
        TEST_EQUAL(task1.proof, 0);
        TEST_FALSE(task2.is_done());
        TEST_EQUAL(task2.proof, 0);
        
        g_mock_time = TimePoint(1ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 2);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_TRUE(scheduler.is_waiting());
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(scheduler.is_busy());
        TEST_FALSE(task1.is_done());
        TEST_EQUAL(task1.proof, 1);
        TEST_FALSE(task2.is_done());
        TEST_EQUAL(task2.proof, 0);

        g_mock_time = TimePoint(2ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 1);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_FALSE(scheduler.is_waiting());
        TEST_TRUE(scheduler.is_busy());
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task1.is_done());
        TEST_EQUAL(task1.proof, 1);
        TEST_FALSE(task2.is_done());
        TEST_EQUAL(task2.proof, 1);

        g_mock_time = TimePoint(3ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 0);
        TEST_EQUAL(scheduler.n_running(), 1);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_FALSE(scheduler.is_waiting());
        TEST_TRUE(scheduler.is_busy());
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task1.is_done());
        TEST_EQUAL(task1.proof, 1);
        TEST_TRUE(task2.is_done());
        TEST_EQUAL(task2.proof, 2);

        g_mock_time = TimePoint(4ms);
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 0);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_TRUE(scheduler.is_waiting());
        TEST_FALSE(scheduler.is_busy());
        TEST_TRUE(scheduler.is_done());
        TEST_TRUE(task1.is_done());
        TEST_EQUAL(task1.proof, 2);
        TEST_TRUE(task2.is_done());
        TEST_EQUAL(task2.proof, 2);

        TEST_END;
    }

    } // private namespace

    

    TestResult Scheduler_test(){
        TEST_GROUP;

        return TestResult()
            | one_task_immediatelly_ready_finishes_instantly
            | one_task_delayed_ready_finishes_instantly
            | one_task_immediatelly_ready_delays
            | two_tasks_first_has_lower_ready_second_has_lower_deadline
            ;
    }

} // namespace fiber
