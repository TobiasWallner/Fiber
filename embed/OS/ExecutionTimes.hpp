#pragma once

/// @brief The measured execution time before the first launch in a cycle and its last suspendsion
struct ExecutionTimes{
    std::chrono::nanoseconds start;     ///< Actual measured cycle-start time of a task
    std::chrono::nanoseconds end;       ///< Actual measured cycle-end time of a task
};