#include "StackAllocator.hpp"

namespace fiber
{
    
    void* StackAllocatorExtern::do_allocate(const std::size_t size, const std::size_t alignment) {
        // compute next propperly aligned storage:
        const std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(&buffer[index]);
        const std::uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
        const std::size_t words_offset = (aligned - addr) / sizeof(word);

        // compute number of words needed to store size many bytes
        const std::size_t words_size = (size + sizeof(word) - 1) / sizeof(word);

        // header size
        const std::size_t words_footer = 1;

        // check if there is enough space
        const std::size_t total_words = words_offset + words_size + words_footer;
        const std::size_t remaining_words = buffer_size - index;

        if(total_words > remaining_words){
            FIBER_THROW(AllocationFailure(size, buffer_size*sizeof(word), remaining_words*sizeof(word)));
        }

        // get result pointer
        void* result = reinterpret_cast<void*>(&buffer[index + words_offset]);

        // write footer
        buffer[index + words_offset + words_size] = words_offset + words_size;

        // advance index
        index += words_offset + words_size + 1;
        return result;
    }

    void StackAllocatorExtern::do_deallocate(void* ptr, [[maybe_unused]]std::size_t bytes, [[maybe_unused]]std::size_t alignment) {
        // read footer
        auto allocated_size = buffer[index-1];

        // check if pointer is in range
        if(!(reinterpret_cast<void*>(&buffer[index-1-allocated_size]) <= ptr && ptr < reinterpret_cast<void*>(&buffer[index]))){
            std::terminate(); // terminate on error
        }

        // perform free
        index = index - allocated_size - 1;
    }

    bool StackAllocatorExtern::do_is_equal([[maybe_unused]] const std::pmr::memory_resource& other ) const noexcept {return false;}
} // namespace fiber


