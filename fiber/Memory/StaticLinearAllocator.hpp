#pragma once

// std
#include <memory_resource>

// fiber
#include <fiber/Exceptions/Exceptions.hpp>
#include <fiber/OStream/OStream.hpp>

namespace fiber
{

    template<size_t N>
    concept is_power_of_two = ((N & (N-1)) == 0);
    /**
     * @brief A linear allocator that uses stack memory and follows `std::pmr::memory_resource`
     * @tparam Bytes The number of bytes (rounded up to multiple of 4)
     */
    template<size_t Bytes>
    struct StaticLinearAllocator : public std::pmr::memory_resource{
        using word = uint32_t;
        static constexpr size_t bufferSize = Bytes / sizeof(word);
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

        void* do_allocate(const std::size_t size, const std::size_t alignment) final {
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
            // TODO: use an fiber error here
            FIBER_THROW(AllocationFailure(size, bufferSize*sizeof(word), largest_free_size*sizeof(word)));
        }

        void do_deallocate(void* ptr, [[maybe_unused]]std::size_t bytes, [[maybe_unused]]std::size_t alignment) final {
            #if (!defined(FIBER_DISABLE_ASSERTIONS) && (defined(FIBER_ASSERTION_LEVEL_CRITICAL) || defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL)))
                // do manual, because deallocate is probably in a destructor and noexcept would call `__exit()` instead of propperly throwing
                if(!(reinterpret_cast<const void*>(&buffer[0]) <= ptr) && (ptr < reinterpret_cast<const void*>(&buffer[bufferSize]))){
                    std::terminate(); // terminate on error
                    return;
                }
            #endif
            Header* header = reinterpret_cast<Header*>(ptr)-1;

            #if (!defined(FIBER_DISABLE_ASSERTIONS) && (defined(FIBER_ASSERTION_LEVEL_CRITICAL) || defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL)))
                std::size_t iterations = 0;
            #endif
            while((header->is_allocated == 0) && (header->size == 0)){
                --header;
                #if (!defined(FIBER_DISABLE_ASSERTIONS) && (defined(FIBER_ASSERTION_LEVEL_CRITICAL) || defined(FIBER_ASSERTION_LEVEL_O1) || defined(FIBER_ASSERTION_LEVEL_FULL)))
                    if(!(reinterpret_cast<const void*>(header) >= reinterpret_cast<const void*>(&buffer[0]))){
                        std::terminate(); // terminate on error
                        return;
                    }
                    if(!(iterations < alignment)){
                        std::terminate(); // terminate on error
                        return;
                    }
                    ++iterations;
                #endif

            }
            header->is_allocated = 0;
        }

        bool do_is_equal([[maybe_unused]] const std::pmr::memory_resource& other ) const noexcept final {return false;}
    };

    template<size_t Bytes>
    class StaticLinearAllocatorDebug : public std::pmr::memory_resource {
    private:
        std::size_t _count_alloc = 0;
        std::size_t _count_free = 0;
        StaticLinearAllocator<Bytes> _allocator;
    public:
        std::size_t nalloc() const {return this->_count_alloc;}
        std::size_t nfree() const {return this->_count_free;}

        template<class Stream>
        inline void dump(Stream& stream) {this->_allocator.dump(stream);}

        void* do_allocate(const std::size_t size, const std::size_t alignment) final {
            ++this->_count_alloc;
            return this->_allocator.do_allocate(size, alignment);
        }

        void do_deallocate(void* ptr, [[maybe_unused]]std::size_t size, [[maybe_unused]]std::size_t alignment) final {
            ++this->_count_free;
            this->_allocator.do_deallocate(ptr, size, alignment);
        }

        inline bool do_is_equal([[maybe_unused]] const std::pmr::memory_resource& other ) const noexcept final {return false;}

        inline bool empty(){return this->_allocator.empty();}
    };
    
} // namespace fiber

