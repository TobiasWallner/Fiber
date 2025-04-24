#include "RealTimeScheduler_test.hpp"

#include <embed/TestFramework/TestFramework.hpp>
#include <embed/OS/Clock.hpp>
#include <embed/OS/RealTimeTask.hpp>
#include <embed/OS/RealTimeScheduler.hpp>

namespace embed
{
    namespace
    {
        
        uint32_t _mock_time = 0;
        uint32_t get_time(){return _mock_time;}

        TestResult one_task_immediatelly_ready_finishes_instantly(){
            TEST_START;

            using MockClock = Clock<uint32_t, std::micro, get_time>;

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

    } // private namespace
    

    TestResult RealTimeScheduler_test(){
        TEST_GROUP;

        return TestResult()
            | one_task_immediatelly_ready_finishes_instantly;
            ;
    }

} // namespace embed
