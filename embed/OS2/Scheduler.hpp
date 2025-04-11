#pragma once

#include "Coroutine.hpp"

namespace embed
{

    class IScheduler{
        virtual void kill(CoTask* ptr)
    }


    template<std::size_t n_tasks>
    class StaticLinearScheduler{
        StaticArrayList<CoTask*, n_tasks> task_list;

    public:

        void add_task(CoTask* task){this->task_list.emplace_back(task);}

        void spin(){
            this->task_list.for_each([](CoTask* task){if(task->await_ready()) task->resume();});
            this->task_list.erase_if([](const CoTask* task){return task->is_done();});
        }
    };
} // namespace embed


