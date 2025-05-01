
#include "Coroutine_test.hpp"

#include <embed/OS/Coroutine.hpp>
#include <embed/TestFramework/TestFramework.hpp>
#include <embed/Memory/Allocator.hpp>
#include <embed/OS/Exit.hpp>
#include <embed/OS/Future.hpp>
#include <embed/OS/TryAwait.hpp>
#include <embed/OS/Delay.hpp>
#include <embed/OS/NextCycle.hpp>
#include <embed/OS/CoSignal.hpp>

namespace{
    embed::TestResult Coroutine_SimpleTask_test(){
        TEST_START;

        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        class SimpleTask : public embed::Task{
            public:
            int result = 0;
            SimpleTask() : embed::Task(this->main(), "SimpleTask"){}
            embed::Coroutine<embed::Exit> main(){
                result = 42;
                co_return embed::Exit::Success;
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
            TEST_EQUAL(task.exit_status(), embed::Exit::Success);
        }

        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    embed::TestResult Coroutine_nested_coroutine(){
        TEST_START;
        class CoroutineSuite{
            public:
            int result = 0;

            embed::Coroutine<int> co_second(){
                co_return 18;
            }
        
            embed::Coroutine<embed::Exit> co_first(){
                this->result = co_await co_second();
                co_return embed::Exit::Success;
            }
        };
    
        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;
        
        CoroutineSuite suit;
        embed::Task task(suit.co_first(), "nested task");
        
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


    embed::TestResult Coroutine_waiting_on_Future_test(){
        TEST_START;
        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        class Task : public embed::Task{
            public:
            int result = 0;
            embed::FuturePromisePair<int> future_promise = embed::make_future_promise<int>();

            Task() : embed::Task(this->main(), "Awaiting Future"){}

            embed::Coroutine<embed::Exit> main(){
                result = co_await embed::TryAwait(future_promise.future);
                co_return embed::Exit::Success;
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
            TEST_EQUAL(task.exit_status(), embed::Exit::Success);
        }

        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    embed::TestResult Coroutine_test_signal(){
        TEST_START;

        using namespace std::chrono_literals;

        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        class Task : public embed::Task{
            public:
            int result = 0;
            embed::FuturePromisePair<int> fp_pair = embed::make_future_promise<int>();
            embed::FuturePromisePair<int> future_promise = embed::make_future_promise<int>();

            Task() : embed::Task(this->main(), "Signaling test"){}

            embed::Coroutine<embed::Exit> main(){
                using namespace std::chrono_literals;

                co_await embed::NextCycle();            // signal to await the next cycle
                co_await embed::Delay(100ns);           // signal to await a delay (implicit) of 1ms
                co_await embed::Delay(200ns, 2ns);      // signal to delay (explicit)
                co_await this->fp_pair.future;          // signal to await a future
                co_return embed::Exit::Success;         // signal should be None
            }
        };

        Task task;

        task.resume(); // co_await embed::NextCycle();
        {
            const embed::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), embed::CoSignal::Type::NextCycle);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // co_await embed::Delay(100ns)
        {
            const embed::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), embed::CoSignal::Type::ImplicitDelay);
            TEST_EQUAL(signal.delay(), 100ns);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // embed::Delay(200ns, 2ns);
        {
            const embed::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), embed::CoSignal::Type::ExplicitDelay);
            TEST_EQUAL(signal.delay(), 200ns);
            TEST_EQUAL(signal.deadline(), 2ns);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // co_await this->fp_pair.future;
        {
            const embed::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), embed::CoSignal::Type::Await);
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

        task.resume(); // co_return embed::Exit::Success;
        {
            const embed::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), embed::CoSignal::Type::None);
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_TRUE(task.is_done());
        }
        task.destroy();

        TEST_TRUE(allocator.empty()); // check for memory leaks

        TEST_END;
    }

}// private namespace

namespace embed{
    embed::TestResult Coroutine_test(){
        TEST_GROUP;

        return embed::TestResult()
            | Coroutine_SimpleTask_test
            | Coroutine_nested_coroutine
            | Coroutine_waiting_on_Future_test
            | Coroutine_test_signal;
    }
}