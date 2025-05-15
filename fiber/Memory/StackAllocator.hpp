#pragma once

// std
#include <memory_resource>

// fiber
#include <fiber/Exceptions/Exceptions.hpp>
#include <fiber/OStream/OStream.hpp>

namespace fiber
{

    class StackAllocatorExtern : public std::pmr::memory_resource{
    public:
        using word = uint32_t;
    private:
        word* const buffer;
        std::size_t const buffer_size;
        std::size_t index = 0;

    public:
        constexpr StackAllocatorExtern(word* buffer, std::size_t buffer_size)
            : buffer(buffer)
            , buffer_size(buffer_size){}

        constexpr empty() const {return index == 0;}

    private:
        void* do_allocate(const std::size_t size, const std::size_t alignment) final;
        void do_deallocate(void* ptr, [[maybe_unused]]std::size_t bytes, [[maybe_unused]]std::size_t alignment) final;
        bool do_is_equal([[maybe_unused]] const std::pmr::memory_resource& other ) const noexcept final;
    };

    template<size_t N>
    class StackAllocator : public StackAllocatorExtern{
        static constexpr std::size_t const buffer_size = (N+sizeof(word)-1)/sizeof(word);
        StackAllocatorExtern::word buffer[buffer_size];

    public:
        StackAllocator() : StackAllocatorExtern(buffer, buffer_size){}

    };
} // namespace fiber
