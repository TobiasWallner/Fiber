#pragma once

#include "embed/Exceptions.hpp"
#include "embed/OStream.hpp"

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

        struct Header{
            uint32_t is_allocated : 1;
            uint32_t size : 31;   
        };

        StaticLinearAllocator(){
            Header* header = reinterpret_cast<Header*>(&buffer[0]);
            header->is_allocated = 0;
            header->size = Bytes / sizeof(uint32_t) - 1;
        }

        inline Header* header(size_t index){
            return reinterpret_cast<Header*>(&buffer[index]);
        }

        template<class Stream>
        void dump(Stream& stream) {
            stream << "==== Memory Dump ====\n";
            size_t index = 0;
            while (index < bufferSize) {
                stream << "index: " << index
                        << " | allocated: " << header(index)->is_allocated
                        << " | size: " << ((header(index)->size) * sizeof(word)) << '\n';
                index += header(index)->size + 1;
            }
            stream << "=====================\n";
        }

        bool empty(){
            if(header(0)->is_allocated) return false;
            this->combine_free(0);
            const bool result = (header(0)->size + 1) == bufferSize;
            return result;
        }

        void combine_free(std::size_t index){
            if(header(index)->is_allocated == 0){// scan and see if memory blocks can be combined
                uint32_t totalFreeSize = 0;
                for(size_t i = index; (i < bufferSize) && (header(i)->is_allocated == 0); i += header(i)->size + 1){
                    totalFreeSize += header(i)->size + 1;
                }
                header(index)->size = totalFreeSize - 1; 
            }
        }

        void* do_allocate(const std::size_t size, const std::size_t alignment) override {
            const std::size_t num_words = (size + sizeof(word) - 1) / sizeof(word);

            std::size_t largest_free_size = 0;
            std::size_t nfree = 0;
            std::size_t nalloc = 0;
            std::size_t index = 0;
            while(index < bufferSize){
                
                combine_free(index);
                
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

                        const size_t next_header_index = index + 1 + size_to_allocate;
                        const size_t next_header_size = old_size - size_to_allocate - 1;

                        header(next_header_index)->is_allocated = 0;
                        header(next_header_index)->size = next_header_size;

                    }
                    // install alignment symbols
                    for(std::size_t i = 0; i < alignment_offset; ++i){
                        header(index+i+1)->is_allocated = 0;
                        header(index+i+1)->size = 0;
                    }
                    return reinterpret_cast<void*>((&buffer[index + 1 + alignment_offset]));
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
            #if defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)
                // do manual, because deallocate is probably in a destructor and noexcept would call `__exit()` instead of propperly throwing
                if(!(reinterpret_cast<const void*>(&buffer[0]) <= ptr) && (ptr < reinterpret_cast<const void*>(&buffer[bufferSize]))){
                    embed::cerr << embed::AssertionFailureCritical("(&buffer[0]) <= ptr) && (ptr < (&buffer[bufferSize])", "Tried to free pointer that is not in the range of the allocator.", EMBED_FUNCTION_SIGNATURE) << embed::endl;
                    return;
                }
            #endif
            Header* header = reinterpret_cast<Header*>(ptr)-1;

            #if defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)
                std::size_t iterations = 0;
            #endif
            while((header->is_allocated == 0) && (header->size == 0)){
                --header;
                #if defined(EMBED_ASSERTION_LEVEL_CRITICAL) || defined(EMBED_ASSERTION_LEVEL_O1) || defined(EMBED_ASSERTION_LEVEL_FULL)
                    if(!(reinterpret_cast<const void*>(header) >= reinterpret_cast<const void*>(&buffer[0]))){
                        embed::cerr << embed::AssertionFailureCritical("reinterpret_cast<const void*>(header) >= reinterpret_cast<const void*>(&buffer[0])", "Tried to free a pointer that is not a valid memory segment within the allocator", EMBED_FUNCTION_SIGNATURE) << embed::endl;
                        return;
                    }
                    if(!(iterations < alignment)){
                        embed::cerr << embed::AssertionFailureCritical("iterations < alignment", "Tried to free a pointer that is not a valid memory segment within the allocator", EMBED_FUNCTION_SIGNATURE) << embed::endl;
                        return;
                    }
                    ++iterations;
                #endif

            }
            header->is_allocated = 0;
        }

        bool do_is_equal([[maybe_unused]] const std::pmr::memory_resource& other ) const noexcept override {return false;}
    };

    template<size_t Bytes>
    class StaticLinearAllocatorDebug : public StaticLinearAllocator<Bytes>{
    private:
        std::size_t count_alloc = 0;
        std::size_t count_free = 0;
    public:
        std::size_t nalloc() const {return this->count_alloc;}
        std::size_t nfree() const {return this->count_free;}

        void* do_allocate(const std::size_t size, const std::size_t alignment) override {
            this->count_alloc += 1;
            return this->StaticLinearAllocator<Bytes>::do_allocate(size, alignment);
        }

        void do_deallocate(void* ptr, [[maybe_unused]]std::size_t size, [[maybe_unused]]std::size_t alignment) override {
            this->count_free += 1;
            this->StaticLinearAllocator<Bytes>::do_deallocate(ptr, size, alignment);
        }
    };
    
} // namespace embed

