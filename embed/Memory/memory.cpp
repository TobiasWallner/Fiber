#include <embed/Memory/memory.hpp>

namespace embed{

    void _memcpy(void* dest, const void* source, size_t count){
        size_t i = 0;
        char* cdest = reinterpret_cast<char*>(dest);
        const char* csource = reinterpret_cast<const char*>(source);
        for(; i < count; ++i, (void)++cdest, (void)++csource){
            *cdest = *csource;
        }
    }

    void _async_memcpy(void* dest, const void* source, size_t count, [[maybe_unused]]Promise<int> promise){
        _memcpy(dest, source, count);
    }

    void(*memcpy)(void* dest, const void* source, size_t count) = _memcpy;
    void(*async_memcpy)(void* dest, const void* source, size_t count, Promise<int> promise) = _async_memcpy;

}