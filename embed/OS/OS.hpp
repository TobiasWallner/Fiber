#pragma once

//std
#include <memory_resource>
#include <chrono>
#include <vector>
#include <coroutine>
#include <queue>

//embed
#include "embed/OS/BaseTask.hpp"
#include "embed/StaticPriorityQueue.hpp"
#include "embed/StaticArrayList.hpp"


// TODO: Shared multi buffer system
// [deadline priority queue][start priority queue][yield bench unordered][await bench unordered][hybernate bench unordered]

/* 
    I know one should not provide namespaces, but it is just tedious for the suffixes.
    So I just provide it here and reduce that friction. It is just for numbers: 100us - come on.
*/
using namespace std::chrono_literals;

namespace embed{

    //TODO: add template allocation
    template<std::chrono::nanoseconds timer_overflow, long MaxNumberOfTasks = -1>
    class OS{
    private:
        std::chrono::nanoseconds (*_time)(void); ///< function pointer tht retreives the time
        std::chrono::nanoseconds _prev_time; ///< previous spin time to see if an overflow of the systems clock happened
        
        StaticPriorityQueue<CoTask*, MaxNumberOfTasks, embed::has_earlier_deadline<timer_overflow>> _ready_queue; ///< tasks that are enabled and ready to be executed
        StaticPriorityQueue<CoTask*, MaxNumberOfTasks, embed::has_earlier_start_time<timer_overflow>> _waiting_queue; ///< tasks that are enabled but have not reached their start time yet
        StaticArrayList<CoTask*, MaxNumberOfTasks> _await_queue; ///< tasks that are awaiting external input
        StaticArrayList<CoTask*, MaxNumberOfTasks> _yield_queue; ///< tasks that are awaiting external input
        StaticArrayList<CoTask*, MaxNumberOfTasks> _hybernate_queue; ///< tasks that are awaiting external input

    public:
        inline embed::OS::OS(std::chrono::nanoseconds (*time)(void)) 
            : _time(time)
            , _prev_time(time()){}

        inline std::chrono::nanoseconds time() const {return this->_time();};

        void add_task(TaskType* task);

        void spin();

    private:

        void spin_ready_queue();

        /// @brief puts all tasks that are not idle any more back into the ready list
        void spin_suspend_bench();

        /// @brief puts all tasks that are past their start time into the ready queue
        void spin_waiting();
    };
}