#pragma once

//std
#include <memory_resource>
#include <chrono>
#include <vector>
#include <coroutine>
#include <queue>

//embed
#include "BaseTask.hpp"

/* 
    I know one should not provide namespaces, but it is just tedious for the suffixes.
    So I just provide it here and reduce that friction. It is just for numbers: 100us - come on.
*/
using namespace std::chrono_literals;

namespace embed{

    //TODO: add template allocation
    class OS{
    /*private:
        using TaskType = BaseTask;

        std::chrono::nanoseconds (*_time)(void); ///< function pointer tht retreives the time
        std::chrono::nanoseconds _prev_time; ///< previous spin time to see if an overflow of the systems clock happened
        
        using start_time_priority = std::priority_queue<TaskType*, std::vector<TaskType*>, embed::greater_start_time>;
        using deadline_priority = std::priority_queue<TaskType*, std::vector<TaskType*>, embed::greater_deadline>;

        deadline_priority _ready_queue; ///< tasks that are enabled and ready to be executed
        start_time_priority _waiting_queue; ///< tasks that are enabled but have not reached their start time yet
        std::vector<TaskType*> _suspend_bench; ///< tasks that are awaiting external input

    public:
        OS(std::chrono::nanoseconds (*time)(void));

        std::chrono::nanoseconds time() const ;

        void add_task(TaskType* task);

        void spin();

    private:

        void spin_ready_queue();

        /// @brief puts all tasks that are not idle any more back into the ready list
        void spin_suspend_bench();

        /// @brief puts all tasks that are past their start time into the ready queue
        void spin_waiting();
        */
    };
}