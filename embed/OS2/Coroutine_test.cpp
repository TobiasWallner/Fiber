
#include "Coroutine_test.hpp"
#include "Coroutine.hpp"

#include "embed/test.hpp"
#include "embed/OS/Allocator.hpp"
namespace{
    void Coroutine_SimpleTask_test(){
        embed::StaticLinearAllocatorDebug<1024> allocator;
        embed::coroutine_frame_allocator = &allocator;

        class SimpleTask : public embed::CoTask{
            public:
            int result = 0;
            SimpleTask() : embed::CoTask("SimpleTask", this->main()){}
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
            TEST_FALSE(task.done());
            TEST_TRUE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 0);
        
            bool resume_result = task.resume();
            TEST_TRUE(resume_result);
        
        
            TEST_FALSE(allocator.empty());
            TEST_TRUE(task.done());
            TEST_FALSE(task.is_resumable());
            TEST_FALSE(task.is_awaiting());
            TEST_EQUAL(task.result, 42);
            TEST_EQUAL(task.exit_status(), embed::Exit::Success);
        }

        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        embed::cout << "  finished: " << __func__ << embed::endl;
    }

    void Coroutine_nested_coroutine(){
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
        embed::CoTask task("nested task", suit.co_first());
        
        TEST_EQUAL(suit.result, 0);

        task.resume();

        TEST_EQUAL(suit.result, 18);
        TEST_FALSE(allocator.empty());
        

        task.destroy();
        // check for memory leaks
        TEST_EQUAL(allocator.nalloc(), allocator.nfree());
        TEST_TRUE(allocator.empty());

        embed::cout << "  finished: " << __func__ << embed::endl;
    }

}// private namespace

void embed::Coroutine_test(){
    embed::cout << "started: " << __func__ << embed::endl;
    Coroutine_SimpleTask_test();
    Coroutine_nested_coroutine();
    embed::cout << "  finished: " << __func__ << embed::newl << embed::endl;
}