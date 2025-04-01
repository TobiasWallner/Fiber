#pragma once

namespace embed
{
    /// @brief Defines in which way a task suspends
    enum class Suspend{
        Yield, ///< suspends execution of the current task but keeps it running, so that it will be executed in the next cycle
        Cycle, ///< suspends execution, puts the task from the running queue in the waiting queue and calculates a new schedule for the next cylce
    };
} // namespace embed
