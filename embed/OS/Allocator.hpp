#pragma once

#include "embed/Exceptions.hpp"

namespace embed
{
    
    template<size_t N>
    concept is_power_of_two = ((N & (N-1)) == 0);
    /**
     * @brief A linear allocator that uses stack memory and follows `std::pmr::memory_resource`
     * @tparam Bytes The number of bytes (rounded up to multiple of 4)
     */
    template<size_t Bytes>
    struct StaticLinearAllocator : std::pmr::memory_resource{
        using word = uint32_t;
        static constexpr size_t bufferSize = Bytes / sizeof(uint32_t);
        word buffer[bufferSize];

         struct alignas(word) Header{
            uint32_t is_allocated : 1;
            uint32_t size : 31;   
        };

        StaticLinearAllocator(){
            Header* header = reinterpret_cast<Header*>(&buffer[0]);
            header->is_allocated = 0;
            header->size = Bytes / sizeof(uint32_t)-1;
        }

        inline Header* header(size_t index){
            return reinterpret_cast<Header*>(&buffer[index]);
        }

        template<class Stream>
        void dump_buffer(Stream& stream) {
            stream << "==== Memory Dump ====\n";
            size_t index = 0;
            while (index < bufferSize) {
                stream << "index: " << index
                        << " | allocated: " << header(index)->is_allocated
                        << " | size: " << ((header(index)->size - 1) * sizeof(word)) << '\n';
                index += header(index)->size + 1;
            }
            stream << "=====================\n";
        }

        void* do_allocate(const std::size_t size, const std::size_t alignment) override {
            const std::size_t num_words = (size + sizeof(word) - 1) / sizeof(word);

            std::size_t largest_free_size = 0;
            std::size_t nfree = 0;
            std::size_t nalloc = 0;
            std::size_t index = 0;
            while(index < bufferSize){
                
                if(header(index)->is_allocated == 0){// scan and see if memory blocks can be combined
                    uint32_t totalFreeSize = 0;
                    for(size_t i = index; (i < bufferSize) && (header(i)->is_allocated == 0); i += header(i)->size + 1){
                        totalFreeSize += header(i)->size + 1;
                    }
                    header(index)->size = totalFreeSize - 1; 
                }
                
                const size_t alignment_offset = (reinterpret_cast<size_t>(&buffer[index+1]) % alignment) / sizeof(word);
                size_t size_to_allocate = num_words + alignment_offset;

                largest_free_size = ((header(index)->size - alignment_offset) > largest_free_size) ? header(index)->size : largest_free_size;

                nfree += (header(index)->is_allocated == 0);
                nalloc += (header(index)->is_allocated == 1);

                if((header(index)->is_allocated == 0) && (size_to_allocate <= header(index)->size)){
            
                    // get old size
                    size_t old_size = header(index)->size;
                    if(size_to_allocate + 2 >= old_size){
                        size_to_allocate = old_size;
                    }

                    // write new header
                    header(index)->is_allocated = 1;
                    header(index)->size = size_to_allocate;

                    if(size_to_allocate < old_size){
                        // write next header

                        const size_t new_header_index = index + 1 + size_to_allocate;
                        const size_t new_header_size = old_size - size_to_allocate - 1;

                        header(new_header_index)->is_allocated = 0;
                        header(new_header_index)->size = new_header_size;

                    }
                    return reinterpret_cast<void*>((header(index + 1 + alignment_offset)));
                }else{
                    // increment header position
                    const size_t increment = header(index)->size + 1;
                    index += increment;
                }
            }
            // TODO: use an embed error here
            EMBED_THROW_CRITICAL(AllocationFailure(size, bufferSize*sizeof(word), largest_free_size*sizeof(word), nfree, nalloc));
        }

        void do_deallocate(void* ptr, [[maybe_unused]]std::size_t bytes, [[maybe_unused]]std::size_t alignment) override {
            Header* const header = reinterpret_cast<Header*>(ptr)-1;
            EMBED_ASSERT_CRITICAL_MSG(header->is_allocated, "Freeing unallocated memory region. S: Check for double free or invalid pointer.");
            EMBED_ASSERT_CRITICAL_MSG((reinterpret_cast<const void*>(&buffer[0]) <= ptr) && (ptr < reinterpret_cast<const void*>(&buffer[bufferSize])), "Freeing pointer not contained by allocator. S: Check for invalid pointer");
            header->is_allocated = 0;
        }

        bool do_is_equal( const std::pmr::memory_resource& other ) const noexcept override {return false;}
    };

    
} // namespace embed

