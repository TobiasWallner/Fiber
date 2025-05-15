#pragma once

#include <fiber/OS/Coroutine.hpp>
#include <fiber/Containers/DualPriorityQueue.hpp>
namespace fiber
{


    /**
     * @brief Scheduler that circularly resumes all tasks
     */
    template<std::size_t n_tasks>
    class LinearScheduler{
        fiber::ArrayList<Task*, n_tasks> task_list;

    public:

        void add_task(Task* task){this->task_list.emplace_back(task);}

        void spin(){
            this->task_list.erase_if([](const Task* task){
                if(task->is_resumable()){
                    fiber::detail::frame_allocator = task->_frame_allocator;
                    task->resume();
                }
                return task->is_done();
            });
        }
    };

} // namespace fiber


