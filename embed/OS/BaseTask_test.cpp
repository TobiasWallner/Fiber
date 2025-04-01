
#include "BaseTask.hpp"
#include "embed/OStream.hpp"
#include "embed/test.hpp"


namespace embed
{

    class BaseTaskTestSuit{
        public:

        static void run_simple_task(){

            // create simple task
            class SimpleTask : public embed::BaseTask{
                public:
            
                    bool executed = false;
            
                    void init(std::chrono::nanoseconds now){
                        this->start_time(now);
                        this->deadline(now + 1us);
                    };
            
                    TaskFuture<embed::Exit> main(){
                        executed = false;
                        co_return embed::Exit::Success;
                    }
            
                    void update_schedule(){};
            };

            SimpleTask task;
            BaseTask* taskptr = &task;

            std::chrono::nanoseconds now = 10ns;
            taskptr->init(now);
            
            TEST_EQUAL(taskptr->start_time(), 10ns);
            TEST_EQUAL(taskptr->deadline(), now + 1us);
            
            TEST_EQUAL(taskptr->is_done(), true);
            TEST_EQUAL(taskptr->is_done(), false);

            TEST_FALSE(taskptr->is_done());
            TEST_TRUE(taskptr->is_done());

        }

    };

    
    namespace tests{
        void BaseTask_test(){
            BaseTaskTestSuit::run_simple_task();
        }
    } // namespace tests

} // namespace embed

