
// std
#include <memory_resource>

// embed
#include "embed/test.hpp"
#include "embed/Future.hpp"
#include "embed/OS/BaseTask.hpp"
#include "embed/OS/Allocator.hpp"
#include "embed/OS/Yield.hpp"
#include "embed/OS/Cycle.hpp"

namespace embed
{    

    embed::Future<int> future_data;
    embed::Promise<int> promise_data = future_data.make_promise();

    // Example coroutine
    class MyTask : public BaseTask{
        
    public:
        MyTask() : BaseTask(this->main()){}

        embed::TaskFuture<embed::Exit> main() {
            embed::cout << "Before: co_await future_data;" << embed::newl;
            co_await future_data;
            embed::cout << "After: co_await future_data;" << embed::newl;

            co_yield Yield();
            embed::cout << "After: co_yield Yield();" << embed::newl;

            co_yield Cycle();
            embed::cout << "After: co_yield Cycle();" << embed::newl;

            co_yield 13ms;
            embed::cout << "After: co_yield 13ms;" << embed::newl;

            embed::cout << "Before: co_return Exit::Success;" << embed::newl;
            co_return Exit::Success;
        }

        Schedule init_schedule([[maybe_unused]]std::chrono::nanoseconds now) {
            return Schedule{.ready = 0ns, .deadline = 0ns};
        }

        Schedule update_schedule([[maybe_unused]]const Schedule& last_schedule, [[maybe_unused]]const ExecutionTimes& execution_times) {
            return Schedule{.ready = 0ns, .deadline = 0ns};
        }
    };

    class BaseTask_TestSuit{
    public:
        static void test_simple_task(){
            // allocate storage for task frame
            StaticLinearAllocator<64> allocator;
            embed::coroutine_frame_allocator = &allocator;
            {
            MyTask my_task;

            BaseTask* task = &my_task;

            TEST_FALSE(task->done());
            TEST_FALSE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_FALSE(task->is_yielding());
            TEST_FALSE(task->is_delaying());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Ready);
            
            task->resume(); // co_await future_data;
            TEST_FALSE(task->done());
            TEST_TRUE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_FALSE(task->is_yielding());
            TEST_FALSE(task->is_ending_cycle());
            TEST_FALSE(task->is_delaying());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Await);
            
            task->resume(); // co_await future_data; 
            TEST_FALSE(task->done());
            TEST_TRUE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Await);
            TEST_FALSE(task->is_yielding());
            TEST_FALSE(task->is_ending_cycle());
            TEST_FALSE(task->is_delaying());

            promise_data.set_value(5); // deliver promised value to future data

            TEST_FALSE(task->is_awaiting()); //is actually cleared before the next resume by the promise

            task->resume(); // co_yield Yield()
            TEST_FALSE(task->done());
            TEST_TRUE(task->is_yielding());
            TEST_FALSE(task->is_ending_cycle());
            TEST_FALSE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_FALSE(task->is_delaying());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Yield);


            task->resume(); // co_yield Cycle()
            TEST_FALSE(task->done());
            TEST_FALSE(task->is_yielding());
            TEST_TRUE(task->is_ending_cycle());
            TEST_FALSE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_FALSE(task->is_delaying());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Cycle);

            task->resume(); // co_yield 1ms;
            TEST_FALSE(task->done());
            TEST_FALSE(task->is_yielding());
            TEST_FALSE(task->is_ending_cycle());
            TEST_FALSE(task->is_awaiting());
            TEST_FALSE(task->is_returning());
            TEST_TRUE(task->is_delaying());
            TEST_EQUAL(task->delay_value(), 13ms);
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Delay);

            task->resume(); // co_return Exit::Success
            TEST_TRUE(task->done());
            TEST_FALSE(task->is_yielding());
            TEST_FALSE(task->is_ending_cycle());
            TEST_FALSE(task->is_awaiting());
            TEST_TRUE(task->is_returning());
            TEST_FALSE(task->is_delaying());
            TEST_EQUAL(task->co_return_type(), CoroutineStatusType::Return);
            TEST_EQUAL(task->return_value(), Exit::Success);
            }
            

            embed::cout << "finished test" << embed::endl;
        }
    };

    void BaseTask_test(){
        BaseTask_TestSuit::test_simple_task();
    }

} // namespace embed

