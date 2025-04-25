#include "RealTimeScheduler_test.hpp"

#include <embed/TestFramework/TestFramework.hpp>
#include <embed/OS/Clock.hpp>
#include <embed/OS/RealTimeTask.hpp>
#include <embed/OS/RealTimeScheduler.hpp>
#include <embed/Memory/Allocator.hpp>

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

    } // private namespace
    

    TestResult RealTimeScheduler_test(){
        TEST_GROUP;

        return TestResult()
            | one_task_immediatelly_ready_finishes_instantly
            | one_task_delayed_ready_finishes_instantly
            ;
    }

} // namespace embed
