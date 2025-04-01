#pragma once

#include "embed/Exceptions.hpp"

namespace embed
{
    
    template<size_t N>
    concept is_power_of_two = ((N & (N-1)) == 0);

    template<size_t Bytes>
    struct StaticLinearMemoryResource{
        using word = uint32_t;
        static constexpr size_t bufferSize = Bytes / sizeof(uint32_t);
        static inline word buffer[bufferSize];
        static inline bool is_initialised = false;

        struct Header{
            uint32_t is_allocated : 1;
            uint32_t size : 31;   
        };

        static inline void init_once(){
            if(!is_initialised){
                Header* header = reinterpret_cast<Header*>(&buffer[0]);
                header->is_allocated = 0;
                header->size = Bytes / sizeof(uint32_t)-1;
                is_initialised = true;
            }
        }

        static inline Header* header(size_t index){
            return reinterpret_cast<Header*>(&buffer[index]);
        }

        static void dump_buffer() {
            std::cout << "==== Memory Dump ====\n";
            size_t index = 0;
            while (index < bufferSize) {
                std::cout << "index: " << index
                        << " | allocated: " << header(index)->is_allocated
                        << " | size: " << header(index)->size << '\n';
                index += header(index)->size + 1;
            }
            std::cout << "=====================\n";
        }

        static void* allocate(const size_t size, const int alignment){
            init_once();

            const size_t num_words = (size + sizeof(word) - 1) / sizeof(word);

            size_t index = 0;
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
            throw std::runtime_error("Allocation Error");
        }

        static void deallocate(void * const ptr){
            Header* const header = reinterpret_cast<Header*>(ptr)-1;
            
            // free header
            header->is_allocated = 0;
        }
    };

    template<class T, class MemoryRessource>
    struct AllocatorDispatcher {
        
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;

        T* allocate( std::size_t n ){
            return reinterpret_cast<T*>(MemoryRessource::allocate(sizeof(T) * n, alignof(T)));
        }

        void deallocate( T* p, [[maybe_unused]]std::size_t n ){
            MemoryRessource::deallocate(p);
        }

        template<class U>
        struct rebind {
            using other = AllocatorDispatcher<U, MemoryRessource>;
        };

    };

    template<class T, std::size_t N>
    using StaticLinearAllocator = AllocatorDispatcher<T, StaticLinearMemoryResource<N>>;
    
} // namespace embed

