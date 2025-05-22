
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
#include <fiber/OS/Task.hpp>

namespace{
    fiber::TestResult Coroutine_SimpleTask_test(){
        TEST_START;

        fiber::StackAllocator<256> allocator;
        fiber::detail::frame_allocator = &allocator;

        class SimpleTask : public fiber::TaskBase{
            public:
            int result = 0;
            SimpleTask(fiber::StackAllocatorExtern* allocator) : fiber::TaskBase("SimpleTask", allocator, SimpleTask::main, this){}
            
            static fiber::Coroutine<fiber::Exit> main(SimpleTask* This){
                This->result = 42;
                co_return fiber::Exit::Success;
            }
        };

        {
            // allocator is empty
            TEST_TRUE(allocator.empty());

            SimpleTask task(&allocator);
            
            // task is instantiated and allocated but not executed yet
            TEST_FALSE(allocator.empty());
            TEST_FALSE(task.is_done());
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 0);
        
            task.resume();
        
            TEST_FALSE(task._frame_allocator->empty());
            TEST_TRUE(task.is_done());
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 42);
            TEST_EQUAL(task.exit_status(), fiber::Exit::Success);
            
            // check for memory leaks
        }
        
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    fiber::TestResult Coroutine_nested_coroutine(){
        TEST_START;

        fiber::StackAllocator<256> allocator;
        fiber::detail::frame_allocator = &allocator;

        class CoroutineSuite{
            public:
            int result = 0;

            fiber::Coroutine<int> co_second(){
                co_return 18;
            }
        
            static fiber::Coroutine<fiber::Exit> co_first(CoroutineSuite& self){
                self.result = co_await self.co_second();
                co_return fiber::Exit::Success;
            }
        };
        
        CoroutineSuite suit;
        fiber::TaskBase task("nested task", &allocator, CoroutineSuite::co_first, suit);
        
        TEST_EQUAL(suit.result, 0);

        task.resume();

        TEST_EQUAL(suit.result, 18);
        TEST_FALSE(task._frame_allocator->empty());
        

        task.destroy();
        // check for memory leaks
        TEST_TRUE(allocator.empty());
        
        TEST_END;
    }


    fiber::TestResult Coroutine_waiting_on_Future_test(){
        TEST_START;

        fiber::StackAllocator<256> allocator;
        fiber::detail::frame_allocator = &allocator;

        class Task : public fiber::TaskBase{
            public:
            int result = 0;
            fiber::FuturePromisePair<int> future_promise = fiber::make_future_promise<int>();

            Task(fiber::StackAllocatorExtern* allocator) : fiber::TaskBase("Awaiting Future", allocator, Task::main, this){}

            static fiber::Coroutine<fiber::Exit> main(Task* This){
                This->result = co_await fiber::TryAwait(This->future_promise.future);
                co_return fiber::Exit::Success;
            }
        };
        {
            Task task(&allocator);

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
            
            task.destroy();

            // check for memory leaks
        }
        
        TEST_TRUE(allocator.empty());

        TEST_END;
    }

    fiber::TestResult Coroutine_test_signal(){
        TEST_START;

        using namespace std::chrono_literals;

        fiber::StackAllocator<256> allocator;
        fiber::detail::frame_allocator = &allocator;

        class Task : public fiber::TaskBase{
            public:
            int result = 0;
            fiber::FuturePromisePair<int> fp_pair = fiber::make_future_promise<int>();

            Task(fiber::StackAllocatorExtern* allocator) : fiber::TaskBase("Signaling test", allocator, Task::main, this){}

            static fiber::Coroutine<fiber::Exit> main([[maybe_unused]]Task* This){
                using namespace std::chrono_literals;

                {co_await fiber::NextCycle();       }     // signal to await the next cycle
                {co_await fiber::Delay(100ms);      }     // signal to await a delay (implicit) of 1ms
                {co_await fiber::Delay(200ms, 2ms); }     // signal to delay (explicit)
                {co_await This->fp_pair.future;     }     // signal to await a future

                {co_return fiber::Exit::Success;    }     // signal should be None
            }
        };

        fiber::cout << "sizeof(fiber::Duration): " << sizeof(fiber::Duration) << fiber::endl;

        Task task(&allocator);

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
            TEST_EQUAL(signal.delay(), 100ms);
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_FALSE(task.is_done());
        }

        task.resume(); // fiber::Delay(200ns, 2ns);
        {
            const fiber::CoSignal signal = task.get_signal();
            TEST_EQUAL(signal.type(), fiber::CoSignal::Type::ExplicitDelay);
            TEST_EQUAL(signal.delay(), 200ms);
            TEST_EQUAL(signal.deadline(), 2ms);
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