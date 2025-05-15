#pragma once

#include "Coroutine.hpp"
#include <fiber/Memory/StackAllocator.hpp>
namespace fiber
{
    template<std::size_t frame_size>
    class Task : public TaskBase{
        StackAllocator<frame_size> _local_frame_allocator;

        public:

        template <class F, class... Args>
        requires 
            std::invocable<F, Args...> &&
            std::same_as<std::invoke_result_t<F, Args...>, Coroutine<fiber::Exit>>
        constexpr Task(std::string_view task_name, F&& function, Args&&... args){
            this->TaskBase::operator=(TaskBase(task_name, &_local_frame_allocator, std::forward<F>(function), std::forward<Args>(args)...));
        }
            
    };
} // namespace fiber
