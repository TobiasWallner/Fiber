#pragma once

#include "Coroutine.hpp"
#include <fiber/Memory/StackAllocator.hpp>
namespace fiber{

    template<std::size_t frame_size>
    class Task : public TaskBase{
        StackAllocator<frame_size> _local_frame_allocator;

        public:

        /**
         * \brief constructor to create a priority-based task that starts immediatelly
         * 
         * Priority based tasks have a lower priority then deadline-based tasks
         * 
         * A higher priority integer number corresponds to a higher execution priority.
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr Task(std::string_view task_name, uint16_t priority, F&& function, Args&&... args)
        {
            this->TaskBase::operator=(TaskBase(task_name, priority, &_local_frame_allocator, std::forward<F>(function), std::forward<Args>(args)...));
        }

        /**
         * \brief constructor to create a priority-based task
         * 
         * Priority based tasks have a lower priority then deadline-based tasks
         * 
         * A higher priority integer number corresponds to a higher execution priority.
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr Task(std::string_view task_name, uint16_t priority, TimePoint ready, F&& function, Args&&... args)
        {
            this->TaskBase::operator=(TaskBase(task_name, priority, ready, &_local_frame_allocator, std::forward<F>(function), std::forward<Args>(args)...));
        }

        /**
         * \brief constructor to create a real-time deadline-based task
         * 
         * deadline based tasks automatically have int_max assigned to their priority (highest priority).
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr Task(std::string_view task_name, TimePoint ready, TimePoint deadline, F&& function, Args&&... args)
        {
            this->TaskBase::operator=(TaskBase(task_name, ready, deadline, &_local_frame_allocator, std::forward<F>(function), std::forward<Args>(args)...));
        }

        /**
         * \brief constructor to create a real-time deadline-based task
         * 
         * deadline based tasks automatically have int_max assigned to their priority (highest priority).
         */
        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr Task(std::string_view task_name, TimePoint ready, Duration deadline, F&& function, Args&&... args)
        {
            this->TaskBase::operator=(TaskBase(task_name, ready, deadline, &_local_frame_allocator, std::forward<F>(function), std::forward<Args>(args)...));
        }
    };  

    
} // namespace fiber
