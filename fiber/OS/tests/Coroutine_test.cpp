
#include "Coroutine_test.hpp"

#include <fiber/OS/Coroutine.hpp>
#include <fiber/TestFramework/TestFramework.hpp>
#include <fiber/Memory/StaticLinearAllocator.hpp>
#include <fiber/OS/Exit.hpp>
#include <fiber/Future/Future.hpp>
#include <fiber/OS/TryAwait.hpp>
#include <fiber/OS/Delay.hpp>
#include <fiber/OS/NextCycle.hpp>
#include <fiber/OS/CoSignal.hpp>

namespace{
    fiber::TestResult Coroutine_SimpleTask_test(){
        TEST_START;

        fiber::StaticLinearAllocatorDebug<1024> allocator;
        fiber::coroutine_frame_allocator = &allocator;

        class SimpleTask : public fiber::Task{
            public:
            int result = 0;
            SimpleTask() : fiber::Task(this->main(), "SimpleTask"){}
            fiber::Coroutine<fiber::Exit> main(){
                result = 42;
                co_return fiber::Exit::Success;
            }
        };

        // allocator is empty
        TEST_TRUE(allocator.empty());
        {
            SimpleTask task;

            // task is instantiated and allocated but not executed yet
            TEST_FALSE(allocator.empty());
            TEST_FALSE(task.is_done());
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 0);
        
            task.resume();
        
            TEST_FALSE(allocator.empty());
            TEST_TRUE(task.is_done());
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 42);
            TEST_EQUAL(task.exit_status(), fiber::Exit::Success);
        }

        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    fiber::TestResult Coroutine_nested_coroutine(){
        TEST_START;
        class CoroutineSuite{
            public:
            int result = 0;

            fiber::Coroutine<int> co_second(){
                co_return 18;
            }
        
            fiber::Coroutine<fiber::Exit> co_first(){
                this->result = co_await co_second();
                co_return fiber::Exit::Success;
            }
        };
    
        fiber::StaticLinearAllocatorDebug<1024> allocator;
        fiber::coroutine_frame_allocator = &allocator;
        
        CoroutineSuite suit;
        fiber::Task task(suit.co_first(), "nested task");
        
        TEST_EQUAL(suit.result, 0);

        task.resume();

        TEST_EQUAL(suit.result, 18);
        TEST_FALSE(allocator.empty());
        

        task.destroy();
        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        TEST_END;
    }


    fiber::TestResult Coroutine_waiting_on_Future_test(){
        TEST_START;
        fiber::StaticLinearAllocatorDebug<1024> allocator;
        fiber::coroutine_frame_allocator = &allocator;

        class Task : public fiber::Task{
            public:
            int result = 0;
            fiber::FuturePromisePair<int> future_promise = fiber::make_future_promise<int>();

            Task() : fiber::Task(this->main(), "Awaiting Future"){}

            fiber::Coroutine<fiber::Exit> main(){
                result = co_await fiber::TryAwait(future_promise.future);
                co_return fiber::Exit::Success;
            }
        };
        {
            Task task;

            TEST_FALSE(task.is_done());
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());

            task.resume();
            
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_done());
            TEST_TRUE(task.is_awaiting());

            // error: task.resume()
            
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_done());
            TEST_TRUE(task.is_awaiting()); // waits on awaitable

            task.future_promise.promise = 55; // set the awaitable

            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_done());
            TEST_FALSE(task.is_awaiting()); // no longer awaiting

            task.resume(); // did resume -> returns true

            TEST_FALSE(task.is_resumable()); // task is done -> no longer resumable
            TEST_TRUE(task.is_done()); // task is done
            TEST_FALSE(task.is_awaiting()); // no longer awaiting

            TEST_EQUAL(task.result, 55) // test if promise passed the value correctly to the future.
            TEST_EQUAL(task.exit_status(), fiber::Exit::Success);
        }

        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    fiber::TestResult Coroutine_test_signal(){
        TEST_START;

        using namespace std::chrono_literals;

        fiber::StaticLinearAllocatorDebug<1024> allocator;
        fiber::coroutine_frame_allocator = &allocator;

        class Task : public fiber::Task{
            public:
            int result = 0;
            fiber::FuturePromisePair<int> fp_pair = fiber::make_future_promise<int>();
            fiber::FuturePromisePair<int> future_promise = fiber::make_future_promise<int>();

            Task() : fiber::Task(this->main(), "Signaling test"){}

            fiber::Coroutine<fiber::Exit> main(){
                using namespace std::chrono_literals;

                co_await fiber::NextCycle();            // signal to await the next cycle
                co_await fiber::Delay(100ns);           // signal to await a delay (implicit) of 1ms
                co_await fiber::Delay(200ns, 2ns);      // signal to delay (explicit)
                co_await this->fp_pair.future;          // signal to await a future
                co_return fiber::Exit::Success;         // signal should be None
            }
        };

        Task task;

        task.resume(); // co_await fiber::NextCycle();
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::NextCycle);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // co_await fiber::Delay(100ns)
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::ImplicitDelay);
            TEST_EQUAL(signal.delay(), 100ns);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // fiber::Delay(200ns, 2ns);
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::ExplicitDelay);
            TEST_EQUAL(signal.delay(), 200ns);
            TEST_EQUAL(signal.deadline(), 2ns);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // co_await this->fp_pair.future;
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::Await);
            TEST_FALSE(task.is_resumable());
            TEST_TRUE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.fp_pair.promise = 88;
        {
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // co_return fiber::Exit::Success;
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::None);
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_TRUE(task.is_done());
        }
        task.destroy();

        TEST_TRUE(allocator.empty()); // check for memory leaks

        TEST_END;
    }

}// private namespace

namespace fiber{
    fiber::TestResult Coroutine_test(){
        TEST_GROUP;

        return fiber::TestResult()
            | Coroutine_SimpleTask_test
            | Coroutine_nested_coroutine
            | Coroutine_waiting_on_Future_test
            | Coroutine_test_signal;
    }
}