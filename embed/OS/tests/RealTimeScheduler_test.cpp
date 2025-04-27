#include "RealTimeScheduler_test.hpp"

#include <embed/TestFramework/TestFramework.hpp>
#include <embed/OS/Clock.hpp>
#include <embed/OS/RealTimeTask.hpp>
#include <embed/OS/RealTimeScheduler.hpp>
#include <embed/Memory/Allocator.hpp>
#include <embed/OS/Delay.hpp>

namespace embed
{
    namespace
    {
        uint32_t g_mock_time = 0;
        uint32_t get_time(){return g_mock_time;}

        TestResult one_task_immediatelly_ready_finishes_instantly(){
            TEST_START;

            g_mock_time = 0;
            using MockClock = Clock<uint32_t, std::micro, get_time>;

            embed::StaticLinearAllocatorDebug<1024> allocator;
            embed::coroutine_frame_allocator = &allocator;

            class CoSuit{
                public:
                int proof = 0;
                Coroutine<Exit> coroutine(){
                    this->proof = 258;
                    co_return Exit::Success;
                }
            };

            CoSuit co_suit;

            RealTimeTask<MockClock> simpleTask(co_suit.coroutine(), "simpleTask", 0us, 1us);

            RealTimeScheduler<MockClock, 1> scheduler;

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
            g_mock_time = 0;
            using MockClock = Clock<uint32_t, std::micro, get_time>;

            embed::StaticLinearAllocatorDebug<1024> allocator;
            embed::coroutine_frame_allocator = &allocator;

            // task setup
            class Task : public RealTimeTask<MockClock>{
                public:
                int proof = 0;

                Task(const char* name, MockClock::duration ready, MockClock::duration deadline) 
                    : RealTimeTask<MockClock>(this->main(), name, ready, deadline){}

                Coroutine<Exit> main(){
                    this->proof = 258;
                    co_return Exit::Success;
                }
            };

            Task simpleTask("simpleTask", 1us, 2us);

            RealTimeScheduler<MockClock, 1, embed::default_sleep_until<MockClock>> scheduler;

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
            ++g_mock_time;
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
        g_mock_time = 0;
        using MockClock = Clock<uint32_t, std::micro, get_time>;

        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        // task setup
        class Task : public RealTimeTask<MockClock>{
            public:
            int proof = 0;

            Task(const char* name, MockClock::duration ready, MockClock::duration deadline) 
                : RealTimeTask<MockClock>(this->main(), name, ready, deadline){}

            Coroutine<Exit> main(){
                this->proof = 1;
                co_await Delay(2us);
                this->proof = 2;
                co_return Exit::Success;
            }
        };

        Task task("Task", 1us, 2us);

        // OutputLogger<MockClock>::stream = embed::cout;
        // RealTimeScheduler<MockClock, 1, embed::default_sleep_until<MockClock>, OutputLogger<MockClock>> scheduler;

        RealTimeScheduler<MockClock, 1> scheduler;

        // added task is not done
        scheduler.add(&task);

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 0);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = 0;
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 0);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = 1;
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 1);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = 2;
        scheduler.spin();

        TEST_EQUAL(scheduler.n_waiting(), 1);
        TEST_EQUAL(scheduler.n_running(), 0);
        TEST_EQUAL(scheduler.n_awaiting(), 0);
        TEST_EQUAL(task.proof, 1);
        TEST_FALSE(scheduler.is_done());
        TEST_FALSE(task.is_done());

        g_mock_time = 3;
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
        g_mock_time = 0;
        using MockClock = Clock<uint32_t, std::micro, get_time>;

        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        // task setup
        class Task : public RealTimeTask<MockClock>{
            /*
            Tests the Earliest-Ready-Time / Earliest-Deadline scheduling priorities
             */
            public:
            int proof = 0;

            Task(const char* name, MockClock::duration ready, MockClock::duration deadline) 
                : RealTimeTask<MockClock>(this->main(), name, ready, deadline){}

            Coroutine<Exit> main(){
                this->proof = 1;
                co_await Delay(0us); // basically yield to other tasks
                this->proof = 2;
                co_return Exit::Success;
            }
        };

        Task task1("Task 1", 1us, 4us);
        Task task2("Task two", 2us, 2us);

        // OutputLogger<MockClock>::stream = embed::cout;
        // RealTimeScheduler<MockClock, 2, embed::default_sleep_until<MockClock>, OutputLogger<MockClock>> scheduler;
        RealTimeScheduler<MockClock, 2> scheduler;

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

        g_mock_time = 0;
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
        
        g_mock_time = 1;
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

        g_mock_time = 2;
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

        g_mock_time = 3;
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

        g_mock_time = 4;
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

    

    TestResult RealTimeScheduler_test(){
        TEST_GROUP;

        return TestResult()
            | one_task_immediatelly_ready_finishes_instantly
            | one_task_delayed_ready_finishes_instantly
            | one_task_immediatelly_ready_delays
            | two_tasks_first_has_lower_ready_second_has_lower_deadline
            ;
    }

} // namespace embed
