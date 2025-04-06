#pragma once


/// @brief A Schedule defines the closed-open [ready, deadline) window in which a task needs to be launched
struct Schedule{
    std::chrono::nanoseconds ready;     ///< Time after which a task's cycle is ready to be launched
    std::chrono::nanoseconds deadline;  ///< Time to which a task's cycle has to have been launched
};