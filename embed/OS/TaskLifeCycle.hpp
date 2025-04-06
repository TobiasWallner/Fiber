#pragma once

#include "embed/OStream.hpp"

namespace embed{

    /**
     * @brief The life cycle states of a task
     * 
     * New -> Created
     * Created -> Waiting | Ready
     * Waiting -> Ready
     * Ready -> Running
     * Running -> Awaiting | Yielding | Delaying | Disabled [1, 2] | ExitSuccess | ExitFailure [3]
     * Awaiting -> Awaiting | ReReady
     * Yielding -> Yielding | ReReady
     * Delaying -> ReReady
     * ReReady -> Running
     * ExitSuccess -> Died
     * ExitFailure -> Killed [4]
     * 
     * 
     * [1] A task that is not running can be disabled by another task.
     * [2] A delayed task can only be enabled by another task
     * [3] A task can also fail due to external reasons, like thrown exceptions or allocation failures
     * [4] A task can also be killed by some external task or signal
     */
    enum class TaskLifeCycle{
        New,            ///< Newly created task --> empty coroutine
        Created,        ///< New task with created co-routine
        Waiting,        ///< waiting to become ready. is in the passive running queue. For tasks that are not in avtive window and wait for their next start time.
        Ready,          ///< ready to be executed the first time this cycle. Is in the active running queue. For tasks which are in the active window [start_time, deadline). will set start execution time
        Running,        ///< current executed task
        Awaiting,       ///< task is waiting on resources
        Yielding,       ///< task is yielding voluntarily
        Delaying,       ///< task that is waiting for time to pass but is still in the same cycle
        ReReady,        ///< task that is ready again. Will **not** set start execution times on resumption
        Disabled,       ///< is in the disabled queue. OS will not touch it until enabled by some external stimulus
        ExitSuccess,    ///< has successfully finished execution - will not be executed again - prepare removal
        ExitFailure,    ///< some error occured that lead to the exit of this task
        Died,           ///< Task died on its own - peacfully - we will send flowers to its parent
        Killed,         ///< someone else killed the task - we will send a special agent to inform the parent
    };

    OStream& operator<<(OStream& stream, TaskLifeCycle value);

}