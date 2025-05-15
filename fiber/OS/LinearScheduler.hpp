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
            this->task_list.for_each([](Task* task){if(task->await_ready()) task->resume();});
            this->task_list.erase_if([](const Task* task){return task->is_done();});
        }
    };

} // namespace fiber


